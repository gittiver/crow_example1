#include "crow/http_response.h"
#include "crow/json.h"
#include <optional>

#define CROW_MAIN

#include "crow.h"

#include "model.hpp"

Model model;

crow::json::wvalue error2json(const std::string &errstring) {
  crow::json::wvalue r;
  r["error"] = errstring;
  return r;
}

// Authentication utilities
bool is_valid(const std::string &username, const std::string &password) {
  return username == "frank" && password == "frank";
}

bool validate_authentication(const crow::request &request, crow::response &response) {
  std::string myauth = request.get_header_value("Authorization");
  if (myauth.empty()) {
    response.code = crow::status::UNAUTHORIZED;
    response.set_header("WWW-Authenticate",
                        "Basic realm=\"User Visible Realm\", charset=\"UTF-8\"");
    return false;
  } else {
    // Cut off starting "Basic "
    std::string mycreds = myauth.substr(6);
    // decode base64
    std::string d_mycreds = crow::utility::base64decode(mycreds, mycreds.size());
    // Now that we have our username:password string,
    // we only need to separate it into 2 different strings
    // and verify their validity:
    size_t found = d_mycreds.find(':');
    std::string username = d_mycreds.substr(0, found);
    std::string password = d_mycreds.substr(found + 1);
    return is_valid(username, password);
  }
}

#define CHECK_AUTHENTICATION(request) \
{ \
  crow::response response; \
  if (!validate_authentication(req, response)) { \
    return response; \
  } \
}
// End of Authentication utilities

#include "bp_user_registration.hpp"

int main() {
  crow::SimpleApp app;
  bp_user_registration bpUserRegistration;

  app.register_blueprint(bpUserRegistration);

  app.port(18080).run();

}
