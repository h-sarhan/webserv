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
