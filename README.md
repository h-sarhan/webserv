<a name="readme-top"></a>


<!-- PROJECT SHIELDS -->
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]


<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/h-sarhan/webserv">
    <img src="logo.png" alt="Logo" width="240" height="240">
  </a>

<h3 align="center">webserv</h3>

  <p align="center">
    An RFC compliant HTTP web server written in C++98
    <br />
    <br />
    <a href="https://github.com/mehrinfirdousi">mfirdous</a>
    ·
    <a href="https://github.com/hsarhan">hsarhan</a>
    ·
    <a href="https://github.com/hadi14250">hakaddou</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#features">Features</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#usage">Usage</a></li>
      </ul>
    </li>
    <li><a href="#license">License</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

![Web Server][product-screenshot]

This project is an implementation of an HTTP web server written in C++. The server can handle incoming multiple concurrent requests from web clients and serve them with HTML files or other static resources, such as images or CSS files. The server also supports the HTTP/1.1 protocol, including features such as persistent connections, chunked encoding, and content compression. In addition, the server includes support for dynamic content generation through CGI scripts, allowing developers to write server-side scripts in languages such as Perl or Python.

This HTTP web server is a highly reliable implementation that remains stable even under extreme loads and with limited system resources. Its efficient design ensures that the server can handle a large number of incoming requests without crashing or hanging


## Features
* Able to handle multiple concurrent requests
* Implements the HTTP protocol including GET, POST, and DELETE requests
* HTTP/1.1 support with persistent connections and chunked encoding
* Static file serving for HTML files, images, and other resources
* CGI script support for dynamic content generation
* Robust and resilient implementation
* Parses a configuration file
* Handles multiple CGIs
* Supports cookies and session management

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

The only prerequisites for this project are a C++ compiler and the Make build system

This project produces no warnings even with strict warning flags on gcc and clang

* To compile
  ```sh
    make
  ```

### Usage

* To run the web server with default configuration
  ```sh
    ./webserv
  ```

* To run the web server with a custom configuration
  ```sh
    ./webserv ./server.conf
  ```



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[stars-shield]: https://img.shields.io/github/stars/h-sarhan/webserv.svg?style=for-the-badge
[stars-url]: https://github.com/h-sarhan/webserv/stargazers
[issues-shield]: https://img.shields.io/github/issues/h-sarhan/webserv.svg?style=for-the-badge
[issues-url]: https://github.com/h-sarhan/webserv/issues
[license-shield]: https://img.shields.io/github/license/h-sarhan/webserv.svg?style=for-the-badge
[license-url]: https://github.com/h-sarhan/webserv/blob/master/LICENSE
[product-screenshot]: https://www.hostinger.com/tutorials/wp-content/uploads/sites/2/2018/06/How-Server-Work.webp
