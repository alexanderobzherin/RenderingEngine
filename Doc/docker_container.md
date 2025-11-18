## Docker (Optional)

1. To run docker without sudo
```bash
sudo groupadd docker
sudo usermod -aG docker $USER
```

2. Build container based on Dockerfile image
```bash
docker build -t re_env_cnr .
```

3. Run container
```bash
docker run --name test_env_cnr -v /home/alexander/Development/rendering-engine:/rendering-engine -it re_env_cnr
```

4. If container already has been run before
```bash
docker start -i re_env_cnr
```

5. Stop container
```bash
docker stop re_env_cnr
```

6. Remove unnecessary image and containers
```bash
docker image prune -a
docker container prune
```

<- [Back to Developer Guide Page](developer_guide.md)