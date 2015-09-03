/*
 * ContentTypeProvider.h
 *
 *  Created on: May 12, 2014
 *      Author: enrico
 */

#ifndef CONTENTTYPEPROVIDER_H_
#define CONTENTTYPEPROVIDER_H_

#include <map>
#include <string>

class ContentTypeProvider {
    public:
        static ContentTypeProvider& getInstance()
        {
            static ContentTypeProvider    instance; // Guaranteed to be destroyed.
                                  // Instantiated on first use.
            return instance;
        }

        std::string get_type(std::string ext);

    private:
        // key ext, value content-type
        std::map<std::string, std::string> content_types;
        ContentTypeProvider();// Constructor? (the {} brackets) are needed here.
        // Dont forget to declare these two. You want to make sure they
        // are unaccessable otherwise you may accidently get copies of
        // your singleton appearing.
        ContentTypeProvider(ContentTypeProvider const&);              // Don't Implement
        void operator=(ContentTypeProvider const&); // Don't implement

};

#endif /* CONTENTTYPEPROVIDER_H_ */
