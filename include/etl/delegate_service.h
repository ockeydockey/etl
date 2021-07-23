///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2019 jwellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef ETL_DELEGATE_SERVICE_INCLUDED
#define ETL_DELEGATE_SERVICE_INCLUDED

#include "platform.h"
#include "nullptr.h"
#include "static_assert.h"
#include "delegate.h"
#include "array.h"

#if ETL_CPP11_NOT_SUPPORTED
  #if !defined(ETL_IN_UNIT_TEST)
    #error NOT SUPPORTED FOR C++03 OR BELOW
  #endif
#else
namespace etl
{
  //***************************************************************************
  /// An indexed delegate service.
  /// \tparam Range  The number of delegates to handle.
  /// \tparam Offset The lowest delegate id value.
  /// \tparam Delegates Pointer to an array of delegate pointers.
  /// The delegate ids must range between Offset and Offset + Range - 1.
  //***************************************************************************
  template <const size_t Range, const size_t Offset = 0U, const etl::delegate<void(size_t)>* Delegates = nullptr>
  class delegate_service
  {
  public:

    //*************************************************************************
    /// Executes the delegate function for the index.
    /// Compile time assert if the id is out of range.
    /// \tparam Id The id of the delegate.
    //*************************************************************************
    template <const size_t Id>
    void call() const
    {
      ETL_STATIC_ASSERT(Id < (Offset + Range), "Callback Id out of range");
      ETL_STATIC_ASSERT(Id >= Offset,          "Callback Id out of range");

      Delegates[Id - Offset](Id);
    }

    //*************************************************************************
    /// Executes the delegate function for the index.
    /// \param id Id of the delegate.
    //*************************************************************************
    void call(const size_t id) const
    {
      if ((id >= Offset) && (id < (Offset + Range)))
      {
        Delegates[id - Offset](id);
      }
      else
      {
        Delegates[Range](id);
      }
    }
  };

  //***************************************************************************
  /// An indexed delegate service.
  /// \tparam Range  The number of delegates to handle.
  /// \tparam Offset The lowest delegate id value.
  /// The delegate ids must range between Offset and Offset + Range - 1.
  //***************************************************************************
  template <const size_t Range, const size_t Offset>
  class delegate_service<Range, Offset, nullptr>
  {
  public:

    //*************************************************************************
    /// Default constructor.
    /// Sets all delegates to the internal default.
    //*************************************************************************
    delegate_service()
    {
      etl::delegate<void(size_t)> default_delegate = etl::delegate<void(size_t)>::create<delegate_service<Range, Offset>, &delegate_service<Range, Offset>::unhandled>(*this);

      lookup.fill(default_delegate);
    }

    //*************************************************************************
    /// Registers a delegate for the specified id.
    /// Compile time assert if the id is out of range.
    /// \tparam Id The id of the delegate.
    /// \param delegate Reference to the delegate.
    //*************************************************************************
    template <const size_t Id>
    void register_delegate(etl::delegate<void(size_t)> callback)
    {
      ETL_STATIC_ASSERT(Id < (Offset + Range), "Callback Id out of range");
      ETL_STATIC_ASSERT(Id >= Offset, "Callback Id out of range");

      lookup[Id - Offset] = callback;
    }

    //*************************************************************************
    /// Registers a delegate for the specified id.
    /// No action if the id is out of range.
    /// \param id       Id of the delegate.
    /// \param delegate Reference to the delegate.
    //*************************************************************************
    void register_delegate(const size_t id, etl::delegate<void(size_t)> callback)
    {
      if ((id >= Offset) && (id < (Offset + Range)))
      {
        lookup[id - Offset] = callback;
      }
    }

    //*************************************************************************
    /// Registers an alternative delegate for unhandled ids.
    /// \param delegate A reference to the user supplied 'unhandled' delegate.
    //*************************************************************************
    void register_unhandled_delegate(etl::delegate<void(size_t)> callback)
    {
      unhandled_delegate = callback;
    }

    //*************************************************************************
    /// Executes the delegate function for the index.
    /// Compile time assert if the id is out of range.
    /// \tparam Id The id of the delegate.
    //*************************************************************************
    template <const size_t Id>
    void call() const
    {
      ETL_STATIC_ASSERT(Id < (Offset + Range), "Callback Id out of range");
      ETL_STATIC_ASSERT(Id >= Offset, "Callback Id out of range");

      lookup[Id - Offset](Id);
    }

    //*************************************************************************
    /// Executes the delegate function for the index.
    /// \param id Id of the delegate.
    //*************************************************************************
    void call(const size_t id) const
    {
      if ((id >= Offset) && (id < (Offset + Range)))
      {
        lookup[id - Offset](id);
      }
      else
      {
        if (unhandled_delegate.is_valid())
        {
          unhandled_delegate(id);
        }
      }
    }

  private:

    //*************************************************************************
    /// The default callback function.
    /// Calls the user defined 'unhandled' callback if it exists.
    //*************************************************************************
    void unhandled(size_t id) const
    {
      if (unhandled_delegate.is_valid())
      {
        unhandled_delegate(id);
      }
    }

    /// The default delegate for unhandled ids.
    etl::delegate<void(size_t)> unhandled_delegate;

    /// Lookup table of delegates.
    etl::array<etl::delegate<void(size_t)>, Range> lookup;
  };
}

#endif
#endif
