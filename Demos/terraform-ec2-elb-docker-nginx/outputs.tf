output "elb_dns_name" {
    value = "${aws_elb.web_server_elb.dns_name}"
}
