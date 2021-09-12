terraform-ec2-elb-docker-nginx
==============================

## This code is an answer for the following contest

Write a terraform configuration which contains the following:

**AWS Setup**
1. An EC2 instance of type t2.micro based on a Debian Stretch Image.
2. A Loadbalancer forwarding incoming requests to the EC2 instance.

The EC2 instance needs to run an Nginx webserver serving one HTML file (just make one up and make the file part of your Github repo). The Nginx server is a Docker container started on the EC2 instance.

## Setup instructions

Clone this repo locally, cd to terraform-ec2-elb-docker-nginx/ and run `terraform init` then `terraform apply`:
```
git clone https://github.com/amemni/devops-example
cd terraform-ec2-elb-docker-nginx
terraform init
terraform apply
```

The terraform script will then ask you to input 2 variables:
- the *AWS region*: needed to specify where to deploy AWS resources, also to select the appropriat Debian AMI id.
- the *keypair name*: needed for ssh access to the EC2 instance(s), most precisely for the "file" and "local-exec" provisionners to be able to upload the index.html file, install docker, pull and run the nginx image on the EC2 instance.
**_Important note_**: make sure the keypair name you provide was created from your public ssh key you have installed under locally under ~/.ssh/ (id_rsa.pub or id_dsa.pub ..etc), the reason is the "file" and "local-exec" provisionners use an ssh-agent authentication by default with the private ssh key registered in your ssh agent.

Example:
```
amemni@amemni-laptop:~/devops-examples/terraform-ec2-elb-docker-nginx$ terraform apply
var.keypair
  EC2 keypair name to create the EC2 instances, example: aymen.memni (created from your public ssh key under ~/.ssh/id_*sa.pub)

  Enter a value: aymen.memni

var.region
  AWS region where to deploy, example: us-east-1

  Enter a value: us-east-2


An execution plan has been generated and is shown below.
Resource actions are indicated with the following symbols:
  + create

Terraform will perform the following actions:

  + aws_elb.web_server_elb
      id:                                     <computed>
      arn:                                    <computed>
... <--- cut long output! --->

  + aws_instance.web_server_instance
      id:                                     <computed>
      ami:                                    "ami-06dfb9abeb4a6afc6"
... <--- cut long output! --->

  + aws_security_group.default_security_group
      id:                                     <computed>
      arn:                                    <computed>
... <--- cut long output! --->

Plan: 3 to add, 0 to change, 0 to destroy.

Do you want to perform these actions?
  Terraform will perform the actions described above.
  Only 'yes' will be accepted to approve.

  Enter a value: yes

aws_security_group.default_security_group: Creating...
  arn:                                   "" => "<computed>"
  description:                           "" => "Default security group for ingress HTTP access"

...
<--- cut long output! --->
... 
 
  zone_id:                                "" => "<computed>"
aws_elb.web_server_elb: Still creating... (10s elapsed)
aws_elb.web_server_elb: Still creating... (20s elapsed)
aws_elb.web_server_elb: Creation complete after 24s (ID: web-server-elb)

Apply complete! Resources: 3 added, 0 changed, 0 destroyed.

Outputs:

elb_dns_name = web-server-elb-332872914.us-east-2.elb.amazonaws.com
```
You should then be able access the web application from the following URL as shown in the output value of elb_dns_name: http://web-server-elb-332872914.us-east-2.elb.amazonaws.com
![application](https://github.com/amemni/devops-examples/blob/master/terraform-ec2-elb-docker-nginx/pictures/screenshot.png)

## Useful commands

- Run `terraform plan` to generate and show an execution plan with applying changes, example:
```
amemni@amemni-laptop:~/devops-examples/terraform-ec2-elb-docker-nginx$ terraform plan
var.keypair
  EC2 keypair name to create the EC2 instances, example: aymen.memni (created from your public ssh key under ~/.ssh/id_*sa.pub)

  Enter a value: aymen.memni

var.region
  AWS region where to deploy, example: us-east-1

  Enter a value: us-east-2

Refreshing Terraform state in-memory prior to plan...
The refreshed state will be used to calculate this plan, but will not be
persisted to local or remote state storage.


------------------------------------------------------------------------

An execution plan has been generated and is shown below.
Resource actions are indicated with the following symbols:
  + create

Terraform will perform the following actions:

  + aws_elb.web_server_elb
      id:                                     <computed>
      arn:                                    <computed>
... <--- cut long output! --->
      tags.Name:                              "web_server_elb"
      zone_id:                                <computed>

  + aws_instance.web_server_instance
      id:                                     <computed>
      ami:                                    "ami-06dfb9abeb4a6afc6"
... <--- cut long output! --->
      volume_tags.%:                          <computed>
      vpc_security_group_ids.#:               <computed>

  + aws_security_group.default_security_group
      id:                                     <computed>
      arn:                                    <computed>
... <--- cut long output! --->
      tags.Name:                              "default_web_server_security_group"
      vpc_id:                                 <computed>


Plan: 3 to add, 0 to change, 0 to destroy.

------------------------------------------------------------------------

Note: You didn't specify an "-out" parameter to save this plan, so Terraform
can't guarantee that exactly these actions will be performed if
"terraform apply" is subsequently run.
```

- Run `terraform destroy` to destroy all resources that are managed by the terraform script, example:
```
amemni@amemni-Latitude-3540:~/Projects/Terraform-bonial$ terraform destroy
var.keypair
  EC2 keypair name to create the EC2 instances, example: aymen.memni (created from your public ssh key under ~/.ssh/id_*sa.pub)

  Enter a value: aymen.memni

var.region
  AWS region where to deploy, example: us-east-1

  Enter a value: us-east-2

Do you really want to destroy all resources?
  Terraform will destroy all your managed infrastructure, as shown above.
  There is no undo. Only 'yes' will be accepted to confirm.

  Enter a value: yes

Destroy complete! Resources: 0 destroyed.
```

- Run `terraform output` to read from the state file and output an output value as defined in outputs.tf, example:
```
amemni@amemni-laptop:~/devops-examples/terraform-ec2-elb-docker-nginx$ terraform output elb_dns_name
web-server-elb-332872914.us-east-2.elb.amazonaws.com
```
The previous command outputs the DNS name of the ELB resource, you can then access the web server at the following URL: http://web-server-elb-332872914.us-east-2.elb.amazonaws.com

### AWS resources

The following AWS resources are created by the terraform script:
1. a default security group to be attached to EC2 instance(s), which should allow:
	- Incoming (ingress) HTTP:80 and SSH:22 traffic from your local computer, needed to be able to access the web page and for the "file" and "local-exec" provisionners, HTTP access is also needed for ELB health checks on port 80,
	- Outgoing (egress) access for EC2 istances to be able to `apt update`, download docker, pull the nginx image ..etc.

**_Note_**: as explained in comments inside the [main.tf](https://github.com/amemni/devops-examples/blob/master/terraform-ec2-elb-docker-nginx/main.tf) file, range 0.0.0.0/0 was allowed for the purpose of this demo and it is not recommend to have your web servers open to the world, make sure to replace that with a restricted range if possible.
```
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
```


2. one or more EC2 instance(s) depending on the count (you can update the count variable to provision more EC2 instances) of type t2.micro (free tier) and using the official Debian 9 AMI, no public IP address is needed and 2 provisionners are executed at the end with an ssh connection to do the following tasks:
	- a file provisionner in order to scp the content of the files/ folder (containing the index.html and junk.png files files) under /tmp/,
	- a local exec provisionner to do the following in order: add the official docker debian repository to apt sources, `apt update`, install and start docker, pull the nginx image and run and nginx container with copying the index.html file on top of /usr/share/nginx/html.
```
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
```

3. an elastic load balancer that's needed to route and distribute incoming traffic on the EC2 instance(s), with an HTTP health check to evaluate if an EC2 instance is healthy or not, and a simple HTTP listener as defined in the [main.tf](https://github.com/amemni/devops-examples/blob/master/terraform-ec2-elb-docker-nginx/main.tf) file.

**_Note_**: for the purpose of this demo, a simple HTTP listener is used, it's recommended to use a secure HTTPS listener for a production setup using a *signed* SSL certificate that can be uploaded in AWS IAM, ACM or manually.
```
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
```
