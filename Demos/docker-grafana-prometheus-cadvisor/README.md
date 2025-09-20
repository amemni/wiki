# docker-grafana-prometheus-cadvisor

## Intro

This code is an answer for the following contest:

> Write a Dockerfile which uses the official ruby image (https://hub.docker.com/ruby/) which contains a ruby program running in an infinite loop and doing the following:
>
> **Program**
>
> 1. Do a 5sec pause.
> 2. Download the JSON file from http://candidateexercise.s3-website-eu-west-1.amazonaws.com/exercise1.yaml
> 3. Generate terraform resources “aws_instance” (https://www.terraform.io/docs/providers/aws/r/instance.html) and “aws_key_pair” (https://www.terraform.io/docs/providers/aws/r/key_pair.html) out of it. The key needs to be used by the instance.
> 4. Print out the terraform resources.
> 
> **Docker Compose**
> 
> Write a docker-compose file containing the following containers:
> 1. Your program: uses your Dockerfile as the build argument and runs in an infinite loop.
> 2. Either a Prometheus (https://hub.docker.com/r/prom/prometheus/) or Graphite (https://hub.docker.com/r/graphiteapp/graphite-statsd/tags/) container - up to you!
> 3. A Cadvisor (https://hub.docker.com/r/google/cadvisor/) container which needs to collect performance data from all containers and writes into the Prometheus or Graphite DB.
> 4. A Grafana (https://hub.docker.com/r/grafana/grafana/) container which visualises the performance data.

## Setup instructions

Clone this repo locally, cd to docker-grafana-prometheus-cadvisor/ and run `docker-compose up -d`:

```
git clone https://github.com/amemni/devops-examples
cd docker-grafana-prometheus-cadvisor/
docker-compose up -d
```

## Useful commands

- You can edit the [.env](https://github.com/amemni/devops-examples/blob/master/docker-grafana-prometheus-cadvisor/.env) file to change the default values of environment variables, mainly the admin username and password for Grafana, else you can pass values for the environment varibales in the command line before `docker-compose up -d`, example:

```
ADMIN_USER=admin ADMIN_PASSWORD=admin docker-compose up -d
amemni@amemni-laptop:~/devops-examples/docker-grafana-prometheus-cadvisor$ ADMIN_USER=admin ADMIN_PASSWORD=admin docker-compose up -d
dockergrafanaprometheuscadvisor_program_1 is up-to-date
dockergrafanaprometheuscadvisor_cadvisor_1 is up-to-date
dockergrafanaprometheuscadvisor_prometheus_1 is up-to-date
dockergrafanaprometheuscadvisor_grafana_1 is up-to-date
```

- Run `docker-compose ps` or `docker ps` to list running comtainers, example:

```
amemni@amemni-laptop:~/devops-examples/docker-grafana-prometheus-cadvisor$ docker-compose ps
WARNING: The ADMIN_USER variable is not set. Defaulting to a blank string.
WARNING: The ADMIN_PASSWORD variable is not set. Defaulting to a blank string.
                    Name                                  Command               State    Ports   
------------------------------------------------------------------------------------------------
dockergrafanaprometheuscadvisor_cadvisor_1     /usr/bin/cadvisor -logtostderr   Up      8080/tcp 
dockergrafanaprometheuscadvisor_grafana_1      bash /setup.sh                   Up      3000/tcp 
dockergrafanaprometheuscadvisor_program_1      ruby program.rb                  Up               
dockergrafanaprometheuscadvisor_prometheus_1   /bin/prometheus --config.f ...   Up      9090/tcp 
```

- Run `docker-compose down` to shut-down the stack, example:

```
amemni@amemni-laptop:~/devops-examples/docker-grafana-prometheus-cadvisor$ docker-compose down
Stopping dockergrafanaprometheuscadvisor_grafana_1 ... done
Stopping dockergrafanaprometheuscadvisor_prometheus_1 ... done
Stopping dockergrafanaprometheuscadvisor_cadvisor_1 ... done
Stopping dockergrafanaprometheuscadvisor_program_1 ... done
Removing dockergrafanaprometheuscadvisor_grafana_1 ... done
Removing dockergrafanaprometheuscadvisor_prometheus_1 ... done
Removing dockergrafanaprometheuscadvisor_cadvisor_1 ... done
Removing dockergrafanaprometheuscadvisor_program_1 ... done
Removing network dockergrafanaprometheuscadvisor_network
```

- Run `docker network list` to list networks, and `docker network inspect dockergrafanaprometheuscadvisor_network` to inspect the bridge network for the stack, example:

```
amemni@amemni-laptop:~/devops-examples/docker-grafana-prometheus-cadvisor$ docker network list
NETWORK ID          NAME                                      DRIVER              SCOPE
8a6bcb8d89da        bridge                                    bridge              local
e68fee64a6c4        dockergrafanaprometheuscadvisor_network   bridge              local
4139c2666f2e        host                                      host                local
03472c25e3a2        none                                      null                local
amemni@amemni-laptop:~/devops-examples/docker-grafana-prometheus-cadvisor$ docker network inspect dockergrafanaprometheuscadvisor_network
[
    {
        "Name": "dockergrafanaprometheuscadvisor_network",
        "Id": "e68fee64a6c4186f977c01d5f1729f58c2c08075ae3a1cf4333c2e28f2d34e63",
        "Created": "2018-10-08T21:00:29.661008803+01:00",
        "Scope": "local",
        "Driver": "bridge",
        "EnableIPv6": false,
        "IPAM": {
            "Driver": "default",
            "Options": null,
            "Config": [
                {
                    "Subnet": "172.18.0.0/16",
                    "Gateway": "172.18.0.1"
                }
            ]
        },
        "Internal": false,
        "Attachable": false,
        "Containers": {
            "28614eae97dbba2a0018d63e1745e1381c4f34be817d9f15a505d6710bfccf47": {
                "Name": "dockergrafanaprometheuscadvisor_program_1",
                "EndpointID": "90c1c2a9fa57c12598afe63c2366d3bc59eb27602b897cd0224cccb57fa70b21",
                "MacAddress": "02:42:ac:12:00:05",
                "IPv4Address": "172.18.0.5/16",
                "IPv6Address": ""
            },
            "b05244e31913b16180f797a30a18a1580b98183fa46be44cdda48581da129c65": {
                "Name": "dockergrafanaprometheuscadvisor_prometheus_1",
                "EndpointID": "a165dd2c707b07bd0fa637f59ab42acd7b7a64c2d788e5121e0658f47a71abb4",
                "MacAddress": "02:42:ac:12:00:04",
                "IPv4Address": "172.18.0.4/16",
                "IPv6Address": ""
            },
            "cc93fe45736b195b803d613f94df717ea220bd672dda1677d0496bd066d2102b": {
                "Name": "dockergrafanaprometheuscadvisor_grafana_1",
                "EndpointID": "cc7317ba29f3bc95afd300ac8e91f079116e63482fb751d485ac1eece48f2d05",
                "MacAddress": "02:42:ac:12:00:02",
                "IPv4Address": "172.18.0.2/16",
                "IPv6Address": ""
            },
            "e37e53bcbab10c4e76ed9d4e96ec1461d764cd6b6433ed3aefdae25949ca3615": {
                "Name": "dockergrafanaprometheuscadvisor_cadvisor_1",
                "EndpointID": "eb11d9635d21b39d9c9b0abc7374b4a986155690a49d82f2d26951aa56015521",
                "MacAddress": "02:42:ac:12:00:03",
                "IPv4Address": "172.18.0.3/16",
                "IPv6Address": ""
            }
        },
        "Options": {},
        "Labels": {}
    }
]
```

After than, if you are not able to reach containers by hostnames locally, you can edit your /etc/hosts and setup entries for grafana, compose, prometheus and the program containers.

## Grafana dashboards

The Grafana dashboard in this demo was taken from the Grafana Labs official & community built dashboards repository with slight changes, most precisely, this [Docker and system monitoring](https://grafana.com/dashboards/893) dashboard by [Thibaut Mottet](https://grafana.com/orgs/moifort).

The Grafana dashboard displays both Docker and system metric, split by Container, all in one dashboard. 

**System metric:** 
- Time up
- Memory usage/swap
- Disk usage
- Load
- Network
- CPU usage
- Disk I/O

**Docker metric:** 
- CPU usage per container
- Sent network per container
- Received network per container
- Memory usage/swap per container

Creating dashboards in Grafana is very simple and can be done by importing read-to-use dashboards defined in a Grafana JSON format from the UI or using the HTTP API. It is also possible to manually create or edit your dashboards from the UI then export to a Grafana JSON format file for others to use. Refer to this [export_import](http://docs.grafana.org/reference/export_import/) reference for more details.

The !(Grafana Labs official & community built dashboards repository)[https://grafana.com/dashboards] contains templates of dashboards for different use cases.

![Containers_monitoring](https://github.com/amemni/devops-examples/blob/master/docker-grafana-prometheus-cadvisor/pictures/grafana_dashboards.png)

If you want to add more dashboards to the demo, make sure to copy a Grafana JSON format file under ./grafana/dashboards/ folder then re-run `docker-compose up -d`.

### The ruby program

The program service uses the [Dockerfile](https://github.com/amemni/devops-examples/blob/master/docker-grafana-prometheus-cadvisor/Dockerfile) to build a ruby image and runs the [program.rb](https://github.com/amemni/devops-examples/blob/master/docker-grafana-prometheus-cadvisor/program.rb) script in an infinite loop as defined in [docker-compose.yml](https://github.com/amemni/devops-examples/blob/master/docker-grafana-prometheus-cadvisor/docker-compose.yml) (restart: always). To display STDOUT ouptut from the program container, simply un `docker-compose logs -ft program` (-f for following output, -t for display timestamps), example:

```
amemni@amemni-laptop:~/devops-examples/docker-grafana-prometheus-cadvisor$ docker-compose logs -ft program
Attaching to dockergrafanaprometheuscadvisor_program_1
program_1     | 2018-10-08T20:09:57.694805860Z 
program_1     | 2018-10-08T20:09:57.694809152Z resource "aws_instance" "web" {
program_1     | 2018-10-08T20:09:57.694813150Z   ami           = "ami-867ceaff"
program_1     | 2018-10-08T20:09:57.694816453Z   instance_type = "t2.micro"
program_1     | 2018-10-08T20:09:57.694819695Z   keyname       = "aws_key_pair.deployer_web.name"
program_1     | 2018-10-08T20:09:57.694823066Z }
program_1     | 2018-10-08T20:09:57.694835390Z 
program_1     | 2018-10-08T20:09:57.694839620Z resource "aws_key_pair" "deployer_app" {
program_1     | 2018-10-08T20:09:57.694845009Z   public_key = "ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQDczshoUJpD2g91JYC6mNinDEwbAsyOajHlYMw/91Nx28KczGt0AFGfEfS+GbR8zQPUy+jCqiL9jJ4TAq+KeDJpQD8VywSUn43GfshsNKZ7Sproo5PENvkPNyxwlqtKTw0dOyfbrOA8fH/RKLNdNMMRmV19b0Rga15/YO+6I2+ICVEttrCNrTQNOwNNBd+WQDe6neZTJmdM+UlS+6WAlkcpajoGXnWOcrT7WYxBMbYlVEezjfzwiMwEa4f9p0gKBnL/kaVidkwmdAzFO7UHUFeU4zbwysws9lh93x4kBesNxnbqB3NiJHMMFcYq3uvsvqRVp7O/JZFB4uvh6yvatTb7 wolf@workstation"
program_1     | 2018-10-08T20:09:57.694852251Z }
program_1     | 2018-10-08T20:09:57.694856151Z 
program_1     | 2018-10-08T20:09:57.694859113Z resource "aws_instance" "app" {
program_1     | 2018-10-08T20:09:57.694862139Z   ami           = "ami-ce76a7b7"
program_1     | 2018-10-08T20:09:57.694865066Z   instance_type = "t2.micro"
program_1     | 2018-10-08T20:09:57.694868208Z   keyname       = "aws_key_pair.deployer_app.name"
program_1     | 2018-10-08T20:09:57.694871287Z }
program_1     | 2018-10-08T20:09:57.694874120Z 
...
```

### cAdvisor and Prometheus

cAdvisor, or as the name says "Container Advisor", gathers resource usage and performance characteristics of all running containers. The data is exported by containers and the docker host and accessible through the following special system and Docker directories as defined in [docker-compose.yaml](https://github.com/amemni/devops-examples/blob/master/docker-grafana-prometheus-cadvisor/docker-compose.yml):

```
  cadvisor
  ...
    volumes:
      - /:/rootfs:ro
      - /var/run:/var/run:rw
      - /sys:/sys:ro
      - /var/lib/docker/:/var/lib/docker:ro
```

Prometheus is a systems and service monitoring system that collects metrics from configured targets at given times, evaluates rule expressions and display results. The Prometheus configuration is stored in ./prometheus/prometheus.yml and has 2 scrape targets with different scrape_interval values :
- the cadvisor service to collect metrics of all containers and the Docker host,
- the prometheus service to collect metrics of the prometheus container itself.

```
...
scrape_configs:
  - job_name: 'cadvisor'
    scrape_interval: 5s
    static_configs:
      - targets: ['cadvisor:8080']

  - job_name: 'prometheus'
    scrape_interval: 10s
    static_configs:
      - targets: ['localhost:9090']
```
