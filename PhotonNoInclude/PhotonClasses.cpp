#define NO_S3D_USING
#include "PhotonClasses.hpp"

# if SIV3D_PLATFORM(WINDOWS)
# if SIV3D_BUILD_TYPE(DEBUG)
#   pragma comment (lib, "Common-cpp/lib/Common-cpp_vc16_debug_windows_mt_x64")
#   pragma comment (lib, "Photon-cpp/lib/Photon-cpp_vc16_debug_windows_mt_x64")
#   pragma comment (lib, "LoadBalancing-cpp/lib/LoadBalancing-cpp_vc16_debug_windows_mt_x64")
# else
#   pragma comment (lib, "Common-cpp/lib/Common-cpp_vc16_release_windows_mt_x64")
#   pragma comment (lib, "Photon-cpp/lib/Photon-cpp_vc16_release_windows_mt_x64")
#   pragma comment (lib, "LoadBalancing-cpp/lib/LoadBalancing-cpp_vc16_release_windows_mt_x64")
# endif
# endif

#include <LoadBalancing-cpp/inc/Client.h>

namespace PSS
{
    Object::Object()
        : m_object{ std::make_unique<ExitGames::Common::Object>() }
    {
    }

    Object::Object(const ExitGames::Common::Object& object_)
        : m_object{ std::make_unique<ExitGames::Common::Object>(object_) }
    {
    }

    Object::Object(const Object& object_)
        : m_object{ std::make_unique<ExitGames::Common::Object>(*object_.m_object) }
    {
    }
    
    Object::~Object()
    {
    }
    
    Object& Object::operator=(const Object& toCopy)
    {
        m_object->operator=(*toCopy.m_object);
        return *this;
    }
    
    bool Object::operator==(const Object& toCompare) const
    {
        return m_object->operator==(*toCompare.m_object);
    }
    
    bool Object::operator!=(const Object& toCompare) const
    {
        return m_object->operator!=(*toCompare.m_object);
    }
    
    nByte Object::getType() const
    {
        return m_object->getType();
    }
    
    nByte Object::getCustomType() const
    {
        return m_object->getCustomType();
    }
    
    const short* Object::getSizes() const
    {
        return m_object->getSizes();
    }
    
    unsigned int Object::getDimensions() const
    {
        return m_object->getDimensions();
    }

    ExitGames::Common::Object& Object::getObject() const
    {
        return *m_object;
    }

    
    Hashtable::Hashtable(const ExitGames::Common::Hashtable& hashtable_)
        : m_hashtable{ std::make_unique<ExitGames::Common::Hashtable>(hashtable_) }
    {
    }

    Hashtable::~Hashtable()
    {
    }

    Hashtable& Hashtable::operator=(const Hashtable& toCopy)
    {
        m_hashtable->operator=(*toCopy.m_hashtable);
        return *this;
    }

    bool Hashtable::operator==(const Hashtable& toCompare) const
    {
        return m_hashtable->operator==(*toCompare.m_hashtable);
    }

    bool Hashtable::operator!=(const Hashtable& toCompare) const
    {
        return m_hashtable->operator!=(*toCompare.m_hashtable);
    }

    const Object& Hashtable::operator[](unsigned int index) const
    {
        return std::move(Object{ m_hashtable->operator[](index) });
    }

    void Hashtable::put(const ExitGames::Common::Hashtable& src)
    {
        m_hashtable->put(src);
    }


    template<typename FKeyType, typename FValueType>
    void Hashtable::put(const FKeyType& key, const FValueType& val)
    {
        m_hashtable->put<FKeyType, FValueType>(key, val);
    }

    template<typename FKeyType>
    void Hashtable::put(const FKeyType& key)
    {
        m_hashtable->put<FKeyType>(key);
    }

    template<typename FKeyType, typename FValueType>
    void Hashtable::put(const FKeyType& key, const FValueType pVal, const short* const sizes)
    {
        m_hashtable->put<FKeyType, FValueType>(key, pVal, sizes);
    }

    template<typename FKeyType>
    const Object* Hashtable::getValue(const FKeyType& key) const
    {
        return m_hashtable->getValue<FKeyType>(key);
    }
    
    unsigned int Hashtable::getSize() const
    {
        return m_hashtable->getSize();
    }

    const s3d::Array<Object> Hashtable::getKeys() const
    {
        s3d::Array<Object> keys;
        for (unsigned int i{}; i < m_hashtable->getSize(); ++i)
        {
            keys.emplace_back(m_hashtable->operator[](i));
        }

        return keys;
    }


    template<typename FKeyType>
    void Hashtable::remove(const FKeyType& key)
    {
        m_hashtable->remove<FKeyType>(key);
    }

    template<typename FKeyType>
    bool Hashtable::contains(const FKeyType& key) const
    {
        m_hashtable->contains<FKeyType>(key);
    }

    void Hashtable::removeAllElements()
    {
        m_hashtable->removeAllElements();
    }


    Player::Player(const ExitGames::LoadBalancing::Player& player_)
        : m_player{ std::make_unique<ExitGames::LoadBalancing::Player>(player_) }
    {
    }

    Player::~Player()
    {
    }

    Player& Player::operator=(const Player& toCopy)
    {
        m_player->operator=(*toCopy.m_player);
        return *this;
    }

    int Player::getNumber() const
    {
        return m_player->getNumber();
    }

    const s3d::String Player::getName() const
    {
        return Utility::ConvertJStringToString(m_player->getName());
    }

    const s3d::String Player::getUserID() const
    {
        return Utility::ConvertJStringToString(m_player->getUserID());
    }

    Hashtable Player::getCustomProperties() const
    {
        return { m_player->getCustomProperties() };
    }

    bool Player::getIsInactive() const
    {
        return m_player->getIsInactive();
    }

    bool Player::getIsMasterClient() const
    {
        return m_player->getIsMasterClient();
    }

    bool Player::operator==(const Player& player) const
    {
        return m_player->operator==(*player.m_player);
    }

    s3d::String Player::toString(bool withTypes, bool withCustomProperties) const
    {
        return Utility::ConvertJStringToString(m_player->toString(withTypes, withCustomProperties));
    }
}