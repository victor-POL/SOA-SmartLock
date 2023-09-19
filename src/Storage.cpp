#include <Preferences.h>


class Storage
{
private:
    Preferences preferences;
public:
    void StoreData(const char *key, const char *val)
    {
        preferences.begin("smartlock", false);
        preferences.putString(key, val);
        preferences.end();
    }

    String ReadData(const char *val, const char *defaultVal)
    {
        preferences.begin("smartlock", false);
        String ret = preferences.getString(val, defaultVal);
        preferences.end();
        return ret;
    }
};