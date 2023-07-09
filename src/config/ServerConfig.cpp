/**
 * @file ServerConfig.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the ServerConfig class
 * @date 2023-07-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "config/ServerConfig.hpp"
#include "config/Tokenizer.hpp"
#include <cassert>
#include <limits>
#include <sys/stat.h>

/**
 * @brief Create a Default Route object
 *
 * @return Route
 */
static Route createDefaultRoute(void)
{
    Route defaultRoute;
    defaultRoute.serveDir = "./assets/web";
    // Max body size is unlimited by default
    defaultRoute.bodySize = std::numeric_limits<size_t>::max();
    defaultRoute.listDirectories = true;
    defaultRoute.directoryFile = "";
    defaultRoute.redirectTo = "";
    // Only GET is allowed by default
    defaultRoute.methodsAllowed.insert(GET);
    return defaultRoute;
}

/**
 * @brief Create a Default Server Block object
 *
 * @return ServerBlock
 */
static ServerBlock createDefaultServerBlock(void)
{
    ServerBlock defaultServerBlock;
    defaultServerBlock.port = 80;
    defaultServerBlock.hostname = "";
    defaultServerBlock.errorPages.insert(
        std::make_pair(404, "./assets/404.html"));
    defaultServerBlock.errorPages.insert(
        std::make_pair(502, "./assets/502.html"));
    defaultServerBlock.routes.insert(std::make_pair("/", createDefaultRoute()));
    return defaultServerBlock;
}

ServerConfig::ServerConfig(void)
{
    _serverBlocks.push_back(createDefaultServerBlock());
    validationTests();
}

ServerConfig::ServerConfig(const std::string &configFile)
{
    ConfigTokenizer tokenizer(configFile);

    const std::vector<Token> &tokens = tokenizer.tokens();
    (void) tokens;
    validationTests();
}

// * Config file Grammar
// CONFIG_FILE := SERVER [SERVER]...

// SERVER := "server" LEFT_BRACE [SRV_OPTION]... LISTEN  [SRV_OPTION]...\
// LOCATION [SRV_OPTION]... RIGHT_BRACE

// LISTEN := "listen" valid_port SEMICOLON
// SRV_OPTION := SERVER_NAME | ERROR_PAGE
// SERVER_NAME := "server_name" valid_hostname SEMICOLON
// ERROR_PAGE := "error_page" valid_error_response valid_HTML_path SEMICOLON
// SEMICOLON := ";"
// LEFT_BRACE := "{"
// RIGHT_BRACE := "}"

// LOCATION := "location" valid_URL LEFT_BRACE [LOC_OPTION]... (TRY_FILES | \
// REDIRECT) [LOC_OPTION]...RIGHT_BRACE

// TRY_FILES := "try_files" valid_dir SEMICOLON
// REDIRECT := "redirect" valid_URL SEMICOLON

// LOC_OPTION := BODY_SIZE | METHODS | DIR_LISTING | DIR_LISTING_FILE | CGI

// BODY_SIZE := "body_size" positive_number SEMICOLON
// METHODS := "methods" ("GET" | "POST" | "DELETE" | "PUT")... SEMICOLON
// DIR_LISTING := "directory_listing" ("true" | "false") SEMICOLON

// DIR_LISTING_FILE := "directory_listing_file" valid_HTML_path SEMICOLON
// CGI := "cgi_extensions" ("php" | "python")... SEMICOLON

// Labels separated by dots
// Each label must have a length between 1 and 63 characters
// Max length of entire hostname has to be less than 254 characters
// Labels can contain the characters a-z A-Z 0-9 '-'
// Labels cannot start with a hyphen
// Labels cannot end with a hyphen

bool ServerConfig::validateHostName(const std::string &hostname) const
{
    // Check the hostname length
    if (hostname.length() > 253)
        return false;

    // Generate labels by splitting on '.'
    std::vector<std::string> labels;

    std::string label;
    std::stringstream hostStream(hostname);
    while (std::getline(hostStream, label, '.'))
        labels.push_back(label);

    // Check if the label count is correct.
    // This will invalidate labels like "weeb..server" and alan.poe.
    const size_t numLabels =
        std::count(hostname.begin(), hostname.end(), '.') + 1;
    if (labels.size() != numLabels)
        return false;

    // ! STILL NEED TO VALIDATE HOSTNAME LABELS
    // const size_t numValidLabels =
    //     std::count_if(labels.begin(), labels.end(), validateLabel);
    return true;
}

/**
 * @brief Checks if an error response code is valid
 *
 * @param respCode The response code as a string
 * @return true if the error response is valid
 */
bool ServerConfig::validateErrorResponse(const std::string &respCode) const
{
    // Check the length
    if (respCode.length() != 3)
        return false;

    // Check the number of digits
    const size_t numDigits =
        std::count_if(respCode.begin(), respCode.end(), ::isdigit);

    if (numDigits != 3)
        return false;

    // Checks if the response is 4XX or 5XX
    return (respCode.front() == '4' || respCode.front() == '5');
}

/**
 * @brief Checks if the string given is a valid path to an HTML file
 *
 * @param htmlFile Path to HTML file
 * @return true if the path is valid
 */
bool ServerConfig::validateHTMLFile(const std::string &htmlFile) const
{
    struct stat info;

    if (htmlFile.empty())
        return false;

    // Check if the filename has a dot
    size_t dotPos = htmlFile.find_first_of('.');
    if (dotPos == std::string::npos)
        return false;

    // Get the extension
    std::string ext = htmlFile.substr(dotPos, htmlFile.length());

    // Transform extension to lowercase
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    // Check if the filename has an html extension
    if (ext != ".html")
        return false;

    // Check if the path to file/dir failed to open
    if (stat(htmlFile.c_str(), &info) != 0)
        return false;

    // Check if the path was to a file rather than a directory
    return info.st_mode & S_IFREG;
}

/**
 * @brief Checks if the string given is a valid directory
 *
 * @param dirPath Path to directory
 * @return true if the path is valid
 */
bool ServerConfig::validateDirectory(const std::string &dirPath) const
{
    struct stat info;

    // Check if the path to file/dir failed to open
    if (dirPath.empty() || stat(dirPath.c_str(), &info) != 0)
        return false;

    // Check if the path was to a directory
    return info.st_mode & S_IFDIR;
}

/**
 * @brief Checks if a port is valid
 *
 * @param portStr Port to validate
 * @return true if port is valid
 */
bool ServerConfig::validatePort(const std::string &portStr) const
{
    size_t port = 0;
    std::stringstream portStream(portStr);

    portStream >> port;

    // Checks if the the conversion failed or if there are additional characters
    if (portStr.empty() || !portStream || !portStream.eof())
        return false;

    return port >= 1 && port <= 65535;
}

// TODO: ADD MORE CHECKS TO URL VALIDATION
/**
 * @brief Check if a URL is valid
 * @todo Add more checks to the URL
 *
 * @param urlStr URL to validate
 * @return true if the url is valid
 */
bool ServerConfig::validateURL(const std::string &urlStr) const
{
    return !urlStr.empty();
}

/**
 * @brief Tests for the different validation functions
 */
void ServerConfig::validationTests(void) const
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
}

ServerConfig::~ServerConfig(void)
{
}
