# Stage 1: Build the Vue.js application
FROM node:lts-alpine AS build

# Set working directory
WORKDIR /app

# Copy package.json and package-lock.json
COPY package*.json ./

# Install dependencies
RUN npm install

# Update Browserslist database (optional, can be removed if pinned in package.json)
RUN npx update-browserslist-db@latest

# Copy the rest of the application code
COPY . .

# Build the app for production
RUN npm run build

# Stage 2: Serve the built application with Nginx
FROM nginx:alpine

# Copy the built dist folder from the build stage
COPY --from=build /app/dist /usr/share/nginx/html

# Copy custom Nginx configuration
COPY nginx.conf /etc/nginx/conf.d/default.conf

# Expose port 8080 (internal container port)
EXPOSE 8080

# Start Nginx
CMD ["nginx", "-g", "daemon off;"]