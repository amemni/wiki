variable "region" {
  description = "AWS region where to deploy, example: us-east-1"
}

variable "keypair" {
  description = "EC2 keypair name to create the EC2 instances, example: aymen.memni (created from your public ssh key under ~/.ssh/id_*sa.pub)"
}

variable "debian_images" {
  //see https://wiki.debian.org/Cloud/AmazonEC2Image/Stretch
  type = "map"
  default = {
    "ap-northeast-1" = "ami-048813c43a892bf4a"
    "ap-northeast-2" = "ami-0b61dc7b9ac9452c7"
    "ap-south-1"     = "ami-02f59cc6982469cd2"
    "ap-southeast-1" = "ami-0a9a79bb079115e9b"
    "ap-southeast-2" = "ami-0abf02e9015527575"
    "ca-central-1"   = "ami-0e825d093523065f9"
    "eu-central-1"   = "ami-0681ed9bb7a58a33d"
    "eu-west-1"      = "ami-0483f1cc1c483803f"
    "eu-west-2"      = "ami-0d9ba70fd9e495233"
    "eu-west-3"      = "ami-0b59b5cf392c3c2b3"
    "sa-east-1"      = "ami-0bd8e4655e2beef08"
    "us-east-1"      = "ami-03006931f694ea7eb"
    "us-east-2"      = "ami-06dfb9abeb4a6afc6"
    "us-west-1"      = "ami-0f0674cb683fcc1f7"
    "us-west-2"      = "ami-0a1fbca0e5b419fd1"
  }
}
