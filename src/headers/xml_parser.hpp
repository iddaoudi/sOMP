/*
 * =====================================================================================
 *
 *       Filename:  xml_parser.hpp
 *    Description:  Platform file parser (.xml format)
 *         Author:  DAOUDI Idriss
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

#include <tinyxml2.h>
#include <fstream>

void xmlParser(int argc, char *argv[])
{
    // Open the file and read it into a vector
    std::ifstream ifs(argv[1], std::ios::in | std::ios::binary | std::ios::ate);
    std::ifstream::pos_type fsize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::vector<char> bytes(fsize);
    ifs.read(&bytes[0], fsize);

    // Create string from vector
    std::string xml_str(&bytes[0], fsize);

    // Skip unsupported statements
    size_t pos = 0;
    while (true)
    {
        pos = xml_str.find_first_of("<", pos);
        if (xml_str[pos + 1] == '?' || // <?xml...
            xml_str[pos + 1] == '!')
        { // <!DOCTYPE...
            // Skip this line
            pos = xml_str.find_first_of("\n", pos);
        }
        else
            break;
    }
    xml_str = xml_str.substr(pos);

    tinyxml2::XMLDocument doc;
    doc.Parse(xml_str.c_str());

    // Parse platform element
    tinyxml2::XMLNode *pPlatform = doc.FirstChild();

    // Parse zone (socket) element
    tinyxml2::XMLElement *pSocket = pPlatform->FirstChildElement("zone");
    if (pSocket == nullptr)
        throw std::invalid_argument("\nZone 'Socket' element not found! Check your XML platform file.");

    // Parse zone (numa) element
    tinyxml2::XMLElement *pListNuma = pSocket->FirstChildElement("zone");
    if (pListNuma == nullptr)
        throw std::invalid_argument("\nZone 'NUMA' element not found! Check your XML platform file.");

    tinyxml2::XMLElement *pListPack = pListNuma->FirstChildElement("zone");
    if (pListPack == nullptr)
    {
        // Number of caches parsing
        tinyxml2::XMLElement *pListNCache = pListNuma->FirstChildElement("prop");
        if (pListNCache == nullptr)
            throw std::invalid_argument("\nCache prop not found! Check your XML platform file. Maybe add L3 cache value?");
        nCaches = std::atoi(pListNCache->Attribute("value"));

        // Cache size parsing
        tinyxml2::XMLElement *pListCache = pListNCache->NextSiblingElement("prop");
        if (pListCache == nullptr)
            throw std::invalid_argument("\nCache prop not found! Check your XML platform file. Maybe add L3 cache value?");
        realCacheSize = std::atoi(pListCache->Attribute("value"));

        // Alpha coefficient parsing (GEMM model)
        tinyxml2::XMLElement *pAlpha = pListCache->NextSiblingElement("prop");
        if (pAlpha == nullptr)
            throw std::invalid_argument("\nAlpha prop not found! Check your XML platform file. Maybe add L3 cache value?");
        alpha = std::stold(pAlpha->Attribute("value"));

        // Beta coefficient parsing (GEMM model)
        tinyxml2::XMLElement *pBeta = pAlpha->NextSiblingElement("prop");
        if (pBeta == nullptr)
            throw std::invalid_argument("\nBeta prop not found! Check your XML platform file. Maybe add L3 cache value?");
        beta = std::stold(pBeta->Attribute("value"));

        // Create the vector of hosts id
        while (pListNuma != nullptr)
        {
            // Parse host element
            tinyxml2::XMLElement *pListHost = pListNuma->FirstChildElement("host");
            if (pListHost == nullptr)
                throw std::invalid_argument("\nHost element not found! Check your XML platform file.");

            while (pListHost != nullptr)
            {
                const char *tmp = NULL;
                // Put the id in tmp variable
                pListHost->QueryStringAttribute("id", &tmp);
                if (tmp[0] == 'C')
                { //For CPUs
                    hostList.push_back(tmp);
                }
                else if (tmp[0] == 'I')
                { //For IMCs
                    cacheList.push_back(tmp);
                }
                else if (tmp[0] == 'R')
                { //For RAM
                    ramList.push_back(tmp);
                }

                // Go to the next host in the platform file
                pListHost = pListHost->NextSiblingElement("host");
            }
            // Go to the next numa in the platform file
            pListNuma = pListNuma->NextSiblingElement("zone");
        }
    }
}
