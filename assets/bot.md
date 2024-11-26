
# Step to create a bot with API (C++98 without third-party library)

## 1. Create a socket

Create a socket to **communicate with the API Server** using `socket()`
The socket is created using the IP address and port of the API server for communication.

## 2. Connect to IRC Server

Create a socket to **communicate with the IRC Server** using `socket()`
Connect to the IRC server using a socket (`connect()`) and listen for incoming requests.  

## 3. Wait for requests from the IRC Server

The bot continuously listens for new requests from the IRC server.

## 4. Process the requests

### a. Validate the request

Determinate if the request received from the Serve is valid to trigger an HTTP request to the API server.

### b. Build the HTTP request

Build the HTTP request in string format.

HTTP request consists of the following components:
- HTTP method (e.g. `GET`)
- Request path (e.g. `/api/v1/query`)
- Host Name
- Necessary Header Information (e.g., `Content-Type`, `Authorization`, etc.)
- If it is a `POST` request, Request body

Example:
```HTTP
GET /articles?include=author HTTP/1.1
Host: example.com
Authorization: Bearer <api_key>
```
Use `sprintf()` or `snprintf()` to build a string.

#### GET method
The `GET` method requests a representation of the specified resource. Requests using `GET` should only retrieve data and should not contain a request [content](https://developer.mozilla.org/en-US/docs/Glossary/HTTP_Content).
source: [HTTP request methods](https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/GET)

### c. Send the request to API server

Send the request to API server with `send()`


### d. Receive the response from API server

Receive the response from API server with `recv()`

> [!CAUTION]
> The bot should wait for some time after sending a request before sending the next one.
> Use event-driven design or have the bot wait for new messages from the IRC server like `select()`  `sleep()`
  
### e. Parse the response

Parse the response from API server in JSON format using string manipulation functions.
The response consists of `headers` and `a body`. The headers and body are usually separated by a blank line.

Example of response (source: https://jsonapi.org/examples/):
```JSON

HTTP/1.1  200 OK
Content-Type: application/vnd.api+json

{
  "data": [{
  "type": "articles",
  "id": "1",
  "attributes": {
    "title": "JSON:API paints my bikeshed!",
    "body": "The shortest article. Ever.",
    "created": "2015-05-22T14:56:29.000Z",
    "updated": "2015-05-22T14:56:28.000Z"
    },
  "relationships": {
    "author": {
      "data": {"id": "42", "type": "people"}
      }
    }
  }],
  "included": [
    {
      "type": "people",
      "id": "42",
      "attributes": {
        "name": "John",
        "age": 80,
        "gender": "male"
      }
    }
  ]
}

```

> [!CAUTION]
>Don't forget to handle errors as connection failures, timeouts, or HTTP error codes.

### f. Send back the response to IRC server

## 5. Close the socket

Before exiting from the program, close the sockets


## Important notes

- **Synchronous communication**: In C++98, asynchronous communication is difficult to implement. The bot will likely perform synchronous communication, meaning it will send a request and wait for the response before continuing with other tasks.

- **Buffer management**: Since responses from API server may be large, proper buffer management is required. If the response is too large to receive in one go, the bot needs to receive in multiple times.
