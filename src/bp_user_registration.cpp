//
// Created by Gulliver on 13.03.24.
//

#include "bp_user_registration.hpp"

bp_user_registration::bp_user_registration()
:  crow::Blueprint("user") {

  CROW_BP_ROUTE((*this), "/user/register")
  ([]() {
    // ...
    return crow::response(crow::status::OK);
  });

  CROW_BP_ROUTE((*this), "/user/profile/<int>")
      ([](int id) {
      //...
        return crow::response(crow::status::OK);
      });
};
