FROM node:24-alpine

WORKDIR /app

COPY package*.json ./

RUN npm install

COPY . .

EXPOSE 3000

RUN yarn add react-scripts

CMD ["yarn", "start"]