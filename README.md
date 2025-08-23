# Shamir's Secret Sharing

## Introduction
A C++ implementation of Shamir's Secret Sharing.

Video Explanation: https://www.youtube.com/watch?v=_EHMd8gpApo

## Description
Suppose there are n participants who all wish to have access to a secret. You don't want to give each person a copy of the secret because it's too risky - someone could leak it. You also don't want to divide the secret into n pieces and give each person a piece, because that is too impractical - all n participants would need to unite their pieces to retrieve the secret.

Shamir's Secret Sharing provides a way to divide a secret into n parts (one for each participant), such that the original secret is recoverable if any k or more participants combine their parts. But if fewer than k participants combine their shares, they gain no information about the secret.

## Disclaimer
This is my implementation of Shamir's Secret Sharing that I did for personal learning, so I cannot guarantee it's perfectly secure. If you want to use this code, please have it reviewed by a security professional first :)

## Running the Code
```
g++ util.cpp SecretSharing.cpp shamirs-secret-sharing.cpp -Wall -Werror -fsanitize=address -o shamirs-secret-sharing
```
```
./shamirs-secret-sharing
```
