What is a Rate Limiter?  
  
A rate limiter is a mechanism used to control the number of requests or operations a user, or a client can perform within a specific time window.
For example: If a system allows a maximum of 100 requests per minute, any request beyond that limit within the same minute would either be throttled (delayed) or rejected outright, typically returning an HTTP 429: Too Many Requests response.  
  
Now, let’s move on to the functional requirements of this project:  
Per-user limit  
Configurable limits  
Time window management  
Thread-safety  
  
Here, I’ve implemented three classes and an interface:  
RateLimiter — Acts as the manager class, mapping users to their respective request buckets and validating requests. 

UserBucket — Represents a per-user request bucket that holds the user’s rate-limiting state.  

TokenBucket — Implements the rate-limiting logic using the Token Bucket algorithm, handling token refill and consumption.  

RateLimitAlgorithm (interface) — Provides an abstraction for various rate-limiting algorithms, making the system extensible  
