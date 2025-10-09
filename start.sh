docker stop rentstuff_ui
docker rm rentstuff_ui
docker run --name rentstuff_ui -d -p 8098:8080 rentstuff_ui:latest