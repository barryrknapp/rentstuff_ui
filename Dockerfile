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

# Stage 2: Serve the built application
FROM node:lts-alpine

# Install http-server globally
RUN npm install -g http-server

# Set working directory
WORKDIR /app

# Copy only the built dist folder from the build stage
COPY --from=build /app/dist ./dist

# Expose port 8080
EXPOSE 8080

# Serve the application
CMD ["http-server", "dist", "-p", "8080"]