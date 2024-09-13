# Overview

Distributron is a lightweight server that coordinates actual server work to be done by clients. This allows for more lightweight servers, as computational work can be offloaded to client machines. This comes with the following drawbacks:
- potential for security vulnerabilities
- potential for leaking sensitive data

Desitributron does the following to mitigate these risks:
- performs work in multiple random clients to verify results and ensure work is completed
- sensitive data is sent as place holders, and replaced in request strings when sent back to the server
- the server makes all the web calls to external apis
- sends fake work to some clients to limit the value of leaked data
- clients sending repetitive requests will be throttled


# Capabilities and Usage
To use the server you need to specify what parts of the data are sensitive. The request can then be cleaned (sensitive data is replaced with placeholders) and the sent to one of the clients for processing. The client will then send the request back to the server with the result and with a possible request for an api call. The server will then make the api call or return the result to the requesting client. 

For data storage, we could split data across multiple clients, but it's likely to be an issue if clients refresh the page or disconnect at the same time. We might need to store data on the server, though we might provide functionality for storing data client side, if it's data specific to that client.

Although, data storage is the largest cost. On aws pricing calculator is was ~$3,000/year for the average DB. The average server was only ~$600/year. It might be worth exploring ways to distribute the data across clients. This will involve encryption overhead though which will slow down the server. Also for sites that clients don't stay on very long, the data storage will be constantly transferring between clients and new clients. Also if there is a server crash, then pretty much everything will be lost as clients disconnect. I think we'd have to require clients to constantly be available to keep the data alive. Or you might have an on prem database to mitigate that cost. This database needs backups though, that are in different locations. Maybe you'd have a way for different companies to basically share their on prem database with other companies to mitigate costs. 


## MVP
- replace sensitive data with placeholders
- send requests to clients for processing
- verify results from multiple clients
- make api calls from the server
- provide a way to store data client side for specific clients
- distributed on prem data storage



