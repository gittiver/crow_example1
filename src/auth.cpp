//
// Created by Gulliver on 12.08.24.
//
#include "auth.hpp"

// Authentication utilities
bool is_user_authenticated(const std::string &username, const std::string &password) {
  return username == "frank" && password == "frank";
}

bool is_bearer_authenticated(const std::string &beaerer) {
  return beaerer == "123456";
}

static bool validate_authentication(const crow::request &request, crow::response &response) {
  std::string myauth = request.get_header_value("Authorization");
  if (myauth.empty()) {
    response.code = crow::status::UNAUTHORIZED;
    response.set_header("WWW-Authenticate",
                        "Basic realm=\"User Visible Realm\", charset=\"UTF-8\"");
    return false;
  }
  if (myauth.find("Basic ") != std::string::npos) {
    // Cut off starting "Basic "
    std::string mycreds = myauth.substr(6);
    // decode base64
    std::string d_mycreds = crow::utility::base64decode(mycreds, mycreds.size());
    // Now that we have our username:password string,
    // we only need to separate it into 2 different strings
    // and verify their validity:
    size_t found = d_mycreds.find(':');
    if (found == std::string::npos) {
      response.code = crow::status::UNAUTHORIZED;
      response.set_header("WWW-Authenticate",
                          "Basic realm=\"User Visible Realm\", charset=\"UTF-8\"");
      return false;
    } else {
      std::string username = d_mycreds.substr(0, found);
      std::string password = d_mycreds.substr(found + 1);
      if (is_user_authenticated(username, password)) {
        return true;
      } else {
        response.code = crow::status::UNAUTHORIZED;
        response.set_header("WWW-Authenticate",
                            "Basic realm=\"User Visible Realm\", charset=\"UTF-8\"");
        return false;
      }
    }
  } else if (myauth.find("Bearer ") != std::string::npos) {
    // Cut off starting "Bearer "
    std::string bearer = myauth.substr(7);
    if (is_bearer_authenticated(bearer)) {
      return true;
    } else {
      response.code = crow::status::UNAUTHORIZED;
      response.set_header("WWW-Authenticate",
                          "Bearer realm=\"User Visible Realm\", charset=\"UTF-8\"");
      return false;
    }
  } else {
    response.code = crow::status::UNAUTHORIZED;
    response.set_header("WWW-Authenticate",
                        "Basic realm=\"User Visible Realm\", charset=\"UTF-8\"");
    return false;
  }
}

void LoginRequiredMiddleware::before_handle(crow::request &req, crow::response &res, context &ctx) const {
  CROW_LOG_DEBUG << "check auth for " << req.url;
  if (!validate_authentication(req, res)) {
    res.end();
  }
}

void LoginRequiredMiddleware::after_handle(crow::request &req, crow::response &res, context &ctx) const {}
