#pragma once

// Copyright 2005-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This example illustrates how to use boost::hash with a custom hash function.
// The implementation is contained in books.cpp

#include <cstddef>
#include <string>
#include <iostream>

namespace library
{
    struct book
    {
        int id;
        std::string author;
        std::string title;

        book(int i, std::string const& a, std::string const& t)
            : id(i), author(a), title(t) {}

        int getId() const { return id; }
        std::string getTitle() const { return title; }
        std::string getAuthor() const { return author; }

        friend std::ostream& operator<<(std::ostream& os, const library::book &book) {
            os<< "ID="<<book.id << ", Author=" << book.author << ", Title=" << book.title << std::endl;
            return os;
        }
        friend std::ostream& operator<<(std::ostream& os, const library::book *book) {
            os<<(library::book)*book;
            return os;
        }
    };

    bool operator==(book const&, book const&);
    std::size_t hash_value(book const&);
}