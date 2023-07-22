/**
 * @file tests.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief Tests for various important functions
 * @date 2023-07-20
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "tests.hpp"
#include "config/Validators.hpp"
#include "requests/Request.hpp"
#include "utils.hpp"
#include <cassert>
#include <cstring>

/**
 * @brief Tests for the different validation functions
 */
void inputValidatorTests()
{
    assert(validateURL("") == false);
    assert(validateURL("/") == true);
    assert(validateURL("microsoft.com") == true);

    assert(validatePort("") == false);
    assert(validatePort("45a") == false);
    assert(validatePort("0") == false);
    assert(validatePort("-1") == false);
    assert(validatePort("-") == false);
    assert(validatePort("345678987654345") == false);
    assert(validatePort("345 ") == false);
    assert(validatePort("    ") == false);
    assert(validatePort("hgvb") == false);
    assert(validatePort("12132hgvb") == false);
    assert(validatePort("345") == true);
    assert(validatePort("65535") == true);
    assert(validatePort("1") == true);

    assert(validateDirectory("") == false);
    assert(validateDirectory("./garfield") == false);
    assert(validateDirectory("urmom") == false);
    assert(validateDirectory("./src") == true);
    assert(validateDirectory("./src/") == true);
    assert(validateDirectory("src") == true);
    assert(validateDirectory("/") == true);

    assert(validateHTMLFile("dfre") == false);
    assert(validateHTMLFile("src") == false);
    assert(validateHTMLFile("src.html") == false);
    assert(validateHTMLFile("404.html") == false);
    assert(validateHTMLFile("assets/404.html") == true);

    assert(validateErrorResponse("322") == false);
    assert(validateErrorResponse("42") == false);
    assert(validateErrorResponse("4s2") == false);
    assert(validateErrorResponse("5s2") == false);
    assert(validateErrorResponse("5sdd") == false);
    assert(validateErrorResponse("5444") == false);
    assert(validateErrorResponse("43") == false);
    assert(validateErrorResponse("69") == false);
    assert(validateErrorResponse("320") == false);
    assert(validateErrorResponse("544") == true);
    assert(validateErrorResponse("400") == true);
    assert(validateErrorResponse("412") == true);
    assert(validateErrorResponse("420") == true);

    assert(validateHostName("w.w.w") == true);
    assert(validateHostName("w.w.w[]") == false);
    assert(validateHostName("") == false);
    assert(validateHostName("|") == false);
    assert(validateHostName(".") == false);
    assert(validateHostName("...") == false);
    assert(validateHostName(".2.2.") == false);
    assert(validateHostName("w.w.w.") == false);
    assert(validateHostName(";;dewf") == false);
    assert(validateHostName("0/0.0.") == false);
    assert(validateHostName("localhost") == true);
    assert(validateHostName("google.com") == true);
}

void chunkerTests()
{
    char body[] = "POST /urmom HTTP/1.1\r\n"
                  "\r\n"
                  "7\r\n"
                  "Mozilla\r\n"
                  "11\r\n"
                  "Developer Network\r\n"
                  "0\r\n"
                  "\r\n";

    Request req1;

    req1.appendToBuffer(body, sizeOfArray(body) - 1);
    req1.parseRequest();
    req1.unchunk();

    assert(strcmp("POST /urmom HTTP/1.1\r\n\r\nMozillaDeveloper Network", req1.buffer()) == 0);
    assert(req1.length() == 48);

    char bigBody[] =
        "POST /urmom HTTP/1.1\r\n"
        "\r\n"
        "2b\r\n"
        "Lorem ipsum dolor sit amet, consectetur adi\r\n"
        "3e\r\n"
        "piscing elit. Sed ut dui euismod, aliquam massa nec, fermentum\r\n"
        "46\r\n"
        " odio. Quisque volutpat venenatis elit, ac feugiat nibh facilisis nec.\r\n"
        "2c\r\n"
        " In sed justo eget sapien fermentum egestas.\r\n"
        "39\r\n"
        " Nullam fringilla, libero sit amet ullamcorper tincidunt,\r\n"
        "3F\r\n"
        " purus felis consectetur dolor, vel tincidunt sapien est id mi.\r\n"
        "44\r\n"
        " Praesent consequat magna eu leo feugiat, vel eleifend nisi finibus.\r\n"
        "45\r\n"
        " Duis vulputate ex vel ex scelerisque, nec pulvinar sapien efficitur.\r\n"
        "4C\r\n"
        " Etiam pellentesque purus eu lacus faucibus, et bibendum mauris ullamcorper.\r\n"
        "50\r\n"
        " Sed dapibus nunc et auctor facilisis. Nunc eu sem vel turpis eleifend tristique\r\n"
        "e\r\n"
        " eget et eros.\r\n"
        "0\r\n";

    Request req2;

    req2.appendToBuffer(bigBody, sizeOfArray(bigBody) - 1);
    req2.parseRequest();
    req2.unchunk();

    assert(std::strcmp(
               "POST /urmom HTTP/1.1\r\n\r\nLorem ipsum dolor sit amet, consectetur adi"
               "piscing elit. Sed ut dui euismod, aliquam massa nec, fermentum"
               " odio. Quisque volutpat venenatis elit, ac feugiat nibh facilisis nec."
               " In sed justo eget sapien fermentum egestas."
               " Nullam fringilla, libero sit amet ullamcorper tincidunt,"
               " purus felis consectetur dolor, vel tincidunt sapien est id mi."
               " Praesent consequat magna eu leo feugiat, vel eleifend nisi finibus."
               " Duis vulputate ex vel ex scelerisque, nec pulvinar sapien efficitur."
               " Etiam pellentesque purus eu lacus faucibus, et bibendum mauris ullamcorper."
               " Sed dapibus nunc et auctor facilisis. Nunc eu sem vel turpis eleifend tristique"
               " eget et eros.",
               req2.buffer()) == 0);
    assert(req2.length() == 670);
    char failBody[] = "POST /urmom HTTP/1.1\r\n\r\n"
                      "7\r\n"
                      "Mozilla"
                      "11\r\n"
                      "Developer Network\r\n"
                      "0\r\n"
                      "\r\n";

    Request req3;
    req3.appendToBuffer(failBody, sizeOfArray(failBody) - 1);
    req3.parseRequest();
    req3.unchunk();

    assert(req3.length() == 62);
    assert(std::strncmp("POST /urmom HTTP/1.1\r\n\r\n"
                        "7\r\n"
                        "Mozilla"
                        "11\r\n"
                        "Developer Network\r\n"
                        "0\r\n"
                        "\r\n",
                        req3.buffer(), req3.length()) == 0);
}
