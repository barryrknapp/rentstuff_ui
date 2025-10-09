FROM node:lts-alpine

# install simple http server for serving static content
RUN npm install -g http-server

# make the 'app' folder the current working directory
WORKDIR /app

# copy both 'package.json' and 'package-lock.json' (if available)
COPY package*.json ./

#uncomment to build on windows
ENV NODE_OPTIONS=--openssl-legacy-provider


# install project dependencies
RUN npm install

RUN npx update-browserslist-db@latest

# copy project files and folders to the current working directory (i.e. 'app' folder)
COPY . .

# build app for production with minification
RUN npm run build --openssl-legacy-provider
RUN npm run lint

EXPOSE 8080
CMD [ "http-server", "dist" ]