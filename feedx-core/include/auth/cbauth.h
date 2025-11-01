#pragma once
#include <string>
#include <unordered_map>

#include "iauth.h"

class CoinbaseAuth : public IAuthProvider
{
   public:
    std::string get_auth_header() override;

   private:
    void load_env_variables();
    std::string get_jwt();
    std::string get_nonce();

    std::string pem_;
    std::string api_key_;

};