provider "aws" {
  region = "${var.region}"
}

resource "aws_security_group" "default_security_group" {
  name        = "web-server-instance"
  description = "Default security group for ingress HTTP access"

  ingress {
    from_port   = "80"
    to_port     = "80"
    protocol    = "tcp"
    cidr_blocks = [ "0.0.0.0/0" ] //NOT recommented, only for the purpose of this exercise !!
  }
  
  ingress {
    from_port   = "22"
    to_port     = "22"
    protocol    = "tcp"
    cidr_blocks = [ "0.0.0.0/0" ] //NOT recommented, only for the purpose of this exercise !!
  }

  egress {
    from_port   = 0
    to_port     = 65535
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"] //Web server instances need be able to download docker, nginx ..etc
  }

  tags {
    Name = "default_web_server_security_group"
  }
}

resource "aws_instance" "web_server_instance" {
  count = 1

  ami             = "${lookup(var.debian_images, var.region)}"
  instance_type   = "t2.micro"
  key_name        = "${var.keypair}"
  security_groups = [ "${aws_security_group.default_security_group.name}" ]
   
  associate_public_ip_address = false
 
  connection  = {
    user     = "admin" //SSH root login is disabled by default on Debian 9
  }

  provisioner "file" {
    source      = "files/"
    destination = "/tmp"
  }

  provisioner "remote-exec" {
    inline = [
    //see https://docs.docker.com/install/linux/docker-ce/debian
      "wget https://download.docker.com/linux/debian/gpg && sudo apt-key add gpg",
      "echo 'deb [arch=amd64] https://download.docker.com/linux/debian '$(lsb_release -cs)' stable' | sudo tee -a /etc/apt/sources.list.d/docker.list",
      "sudo apt update",
      "sudo apt -y install docker-ce",
      "sudo systemctl start docker",
      "sudo docker pull nginx",
      "sudo docker run -d -p 80:80 -v /tmp:/usr/share/nginx/html nginx"
    ]
  }

  tags {
    Name = "web_server_instance_${count.index}"
  }
}

resource "aws_elb" "web_server_elb" {
  name               = "web-server-elb"
  availability_zones = [ "${aws_instance.web_server_instance.*.availability_zone}" ]
  instances          = ["${aws_instance.web_server_instance.*.id}"]

  listener {
    instance_port     = 80
    instance_protocol = "http"
    lb_port           = 80
    lb_protocol       = "http"
  }

  health_check {
    healthy_threshold   = 2
    unhealthy_threshold = 2
    timeout             = 3
    target              = "HTTP:80/"
    interval            = 30
  }

  idle_timeout                = 400
  connection_draining         = true
  connection_draining_timeout = 400

  tags {
    Name = "web_server_elb"
  }
}
