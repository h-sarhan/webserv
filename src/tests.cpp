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
    char body[] = "7\r\n"
                  "Mozilla\r\n"
                  "11\r\n"
                  "Developer Network\r\n"
                  "0\r\n"
                  "\r\n";

    char bigBody[] =
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

    char *unChunkedBody = unchunker(body, sizeOfArray(body));
    char *unChunkedBigBody = unchunker(bigBody, sizeOfArray(bigBody));
    assert(std::strcmp(unChunkedBody, "Mozilla"
                                      "Developer Network") == 0);
    assert(std::strcmp(
               unChunkedBigBody,
               "Lorem ipsum dolor sit amet, consectetur adi"
               "piscing elit. Sed ut dui euismod, aliquam massa nec, fermentum"
               " odio. Quisque volutpat venenatis elit, ac feugiat nibh facilisis nec."
               " In sed justo eget sapien fermentum egestas."
               " Nullam fringilla, libero sit amet ullamcorper tincidunt,"
               " purus felis consectetur dolor, vel tincidunt sapien est id mi."
               " Praesent consequat magna eu leo feugiat, vel eleifend nisi finibus."
               " Duis vulputate ex vel ex scelerisque, nec pulvinar sapien efficitur."
               " Etiam pellentesque purus eu lacus faucibus, et bibendum mauris ullamcorper."
               " Sed dapibus nunc et auctor facilisis. Nunc eu sem vel turpis eleifend tristique"
               " eget et eros.") == 0);
    delete[] unChunkedBody;
    delete[] unChunkedBigBody;
    char failBody[] = "7"
                      "11\r\n"
                      "Developer Network\r\n"
                      "0\r\n"
                      "\r\n";
    // (void)
    char *unchunkedFailBody = unchunker(failBody, sizeOfArray(failBody));
    (void) unchunkedFailBody;
    assert(unchunkedFailBody == NULL);
}
