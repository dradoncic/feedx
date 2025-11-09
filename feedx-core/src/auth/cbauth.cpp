#include "auth/cbauth.h"

#include <jwt-cpp/jwt.h>
#include <openssl/sha.h>

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <random>

std::string CoinbaseAuth::get_auth_header()
{
    return get_jwt();
}

std::string CoinbaseAuth::get_jwt()
{
    auto token = jwt::create()
                     .set_issuer("coinbase-cloud")
                     .set_subject(api_key_)
                     .set_not_before(std::chrono::system_clock::now())
                     .set_expires_at(std::chrono::system_clock::now() +
                                     std::chrono::seconds{120})
                     .set_header_claim("kid", jwt::claim(std::string(api_key_)))
                     .set_header_claim("nonce", jwt::claim(get_nonce()))
                     .sign(jwt::algorithm::es256{"", priv_key_});

    return token;
}

std::string CoinbaseAuth::get_nonce()
{
    unsigned char bytes[16];
    std::random_device rd;
    for (int i = 0; i < 16; i++)
    {
        bytes[i] = rd() % 256;
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(bytes, 16, hash);

    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return oss.str();
}

void CoinbaseAuth::load_env_variables()
{
    const char* priv_key_c = std::getenv("COINBASE_PRIV_KEY");
    const char* api_key_c = std::getenv("COINBASE_API_KEY");

    if (!priv_key_c || !api_key_c)
    {
        std::cerr << "Necessary keys not set in env.\n";
        return;
    }

    std::string priv_key(priv_key_c);
    std::string api_key(api_key_c);

    size_t pos = 0;
    while ((pos = priv_key.find("\\n", pos)) != std::string::npos)
    {
        priv_key.replace(pos, 2, "\n");
        pos += 1;
    }

    priv_key_ = priv_key;
    api_key_ = api_key;
}