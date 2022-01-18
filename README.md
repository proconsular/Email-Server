# Email Server

## Overview

A simple SMTP mail server written in C++. This is based on my web server project. 
It takes the web server and adds functionality to send and receive emails. It also connects to a MySQL database to track metadata for accounts, emails, etc.

It has a REST api so a user can login and receive a token, whereby they can be authorized to send emails or read their inbox, securely over ssl.

It also features a working-in-progress client written in pure Javascript. Although, I will probably rewrite it in React.

## Goal

My goal with this project was to first see if I could write an email server. Then I wanted to make it as close to a full flegded email server that I could use for other purposes.

This required a lot of work and I had to meticulously read the relevant RFCs (ie rfc5322, rfc5585, etc).
I learned quite a bit about how the email system works from reading the internet standards, especially the particulars regarding ensuring your mail is not viewed as spam.
In order avoid having your mail marked as spam or not even allowed by the destination SMTP server, you must ensure you have three authenication methods set up: SPF, DKIM, and DMARC.
DKIM is probably the trickest since it is essentially a seal on the email, you have to calculate a hash over some of the headers and body and add that as a header field.

Nevertheless, it works in its current state. I aim to rewrite it in a cleaner form now that I understand what are the necessary components to make it work well.
