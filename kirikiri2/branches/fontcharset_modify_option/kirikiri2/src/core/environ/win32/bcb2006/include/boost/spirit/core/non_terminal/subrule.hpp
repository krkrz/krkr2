/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2002-2003 Joel de Guzman
    Copyright (c) 2002-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#if !defined(BOOST_SPIRIT_SUBRULE_HPP)
#define BOOST_SPIRIT_SUBRULE_HPP

#include "boost/type_traits.hpp"
#include "boost/static_assert.hpp"

#if !defined(BOOST_SPIRIT_PARSER_HPP)
#include "boost/spirit/core/parser.hpp"
#endif

#if !defined(BOOST_SPIRIT_PARSER_CONTEXT_HPP)
#include "boost/spirit/core/non_terminal/parser_context.hpp"
#endif

#if !defined(BOOST_SPIRIT_SUBRULE_IPP)
#include "boost/spirit/core/non_terminal/impl/subrule.ipp"
#endif

namespace boost { namespace spirit {

    ///////////////////////////////////////////////////////////////////////////
    //
    //  subrules_scanner class
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename ScannerT, typename ListT>
    struct subrules_scanner : public ScannerT
    {
        typedef ScannerT                            scanner_t;
        typedef ListT                               list_t;
        typedef subrules_scanner<ScannerT, ListT>   self_t;

        subrules_scanner(ScannerT const& scan, ListT const& list_)
        : ScannerT(scan), list(list_) {}

        template <typename PoliciesT>
        struct rebind_policies
        {
            typedef typename rebind_scanner_policies<ScannerT, PoliciesT>::type
                rebind_scanner;
            typedef subrules_scanner<rebind_scanner, ListT> type;
        };

        template <typename PoliciesT>
        subrules_scanner<
            typename rebind_scanner_policies<ScannerT, PoliciesT>::type,
            ListT>
        change_policies(PoliciesT const& policies) const
        {
            typedef subrules_scanner<
                BOOST_SPIRIT_TYPENAME
                    rebind_scanner_policies<ScannerT, PoliciesT>::type,
                ListT>
            subrules_scanner_t;

            return subrules_scanner_t(
                    ScannerT::change_policies(policies),
                    list);
        }

        template <typename IteratorT>
        struct rebind_iterator
        {
            typedef typename rebind_scanner_iterator<ScannerT, IteratorT>::type
                rebind_scanner;
            typedef subrules_scanner<rebind_scanner, ListT> type;
        };

        template <typename IteratorT>
        subrules_scanner<
            typename rebind_scanner_iterator<ScannerT, IteratorT>::type,
            ListT>
        change_iterator(IteratorT const& first, IteratorT const &last) const
        {
            typedef subrules_scanner<
                BOOST_SPIRIT_TYPENAME
                    rebind_scanner_iterator<ScannerT, IteratorT>::type,
                ListT>
            subrules_scanner_t;

            return subrules_scanner_t(
                    ScannerT::change_iterator(first, last),
                    list);
        }

        ListT const& list;
    };

    ///////////////////////////////////////////////////////////////////////////
    //
    //  subrule_scanner type computer class
    //
    //      This computer ensures that the scanner will not be recursively
    //      instantiated if it's not needed.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename ScannerT, typename ListT>
    struct subrules_scanner_finder
    {
          typedef subrules_scanner<ScannerT, ListT> type;
    };

    #if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
    //
    //  MSVC doesn't support PTS. The default implementation
    //  still works. It just generates a whole lot more
    //  template instantiations.

    template <typename ScannerT, typename ListT>
    struct subrules_scanner_finder<subrules_scanner<ScannerT, ListT>, ListT>
    {
          typedef subrules_scanner<ScannerT, ListT> type;
    };

    #endif

    ///////////////////////////////////////////////////////////////////////////
    //
    //  subrule_list class
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename FirstT, typename RestT>
    struct subrule_list : public parser<subrule_list<FirstT, RestT> >
    {
        typedef subrule_list<FirstT, RestT> self_t;
        typedef FirstT                      first_t;
        typedef RestT                       rest_t;

        subrule_list(FirstT const& first_, RestT const& rest_)
        : first(first_), rest(rest_) {}

        template <typename ScannerT>
        struct result
        {
            typedef typename FirstT::def_t parser_t;
            typedef typename parser_result<parser_t, ScannerT>::type type;
        };

        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
                typedef typename subrules_scanner_finder<ScannerT, self_t>::type
                subrules_scanner_t;
            subrules_scanner_t g_arg(scan, *this);
            return first.rhs.parse(g_arg);
        }

        template <int ID, typename DefT, typename ContextT>
        subrule_list<
            FirstT,
            subrule_list<
                subrule_parser<ID, DefT, ContextT>,
                RestT> >
        operator,(subrule_parser<ID, DefT, ContextT> const& rhs)
        {
            return subrule_list<
                FirstT,
                subrule_list<
                    subrule_parser<ID, DefT, ContextT>,
                    RestT> >(
                        first,
                        subrule_list<
                            subrule_parser<ID, DefT, ContextT>,
                            RestT>(rhs, rest));
        }

        FirstT first;
        RestT rest;
    };

    ///////////////////////////////////////////////////////////////////////////
    //
    //  subrule_parser class
    //
    ///////////////////////////////////////////////////////////////////////////
    template <int ID, typename ContextT = parser_context>
    struct subrule; // Forward declaration

    template <int ID, typename DefT, typename ContextT>
    struct subrule_parser
    : public parser<subrule_parser<ID, DefT, ContextT> >
    {
        typedef subrule_parser<ID, DefT, ContextT> self_t;
        typedef subrule<ID, ContextT> subrule_t;
        typedef DefT def_t;

        BOOST_STATIC_CONSTANT(int, id = ID);

        template <typename ScannerT>
        struct result
        {
            typedef typename
                impl::get_subrule_parser_result<
                    DefT, ScannerT, typename subrule_t::attr_t>::type type;
        };

        subrule_parser(subrule_t const& start_, DefT const& rhs_)
        : rhs(rhs_), start(start_) {}

        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            // This will only be called when parsing single subrules.
            typedef subrule_list<self_t, nil_t> list_t;
            typedef subrules_scanner<ScannerT, list_t> scanner_t;

            list_t    list(*this, nil_t());
            scanner_t g_arg(scan, list);
            return start.parse(g_arg);
        }

        template <int ID2, typename DefT2, typename ContextT2>
        inline subrule_list<
            self_t,
            subrule_list<
                subrule_parser<ID2, DefT2, ContextT2>,
                nil_t> >
        operator,(subrule_parser<ID2, DefT2, ContextT2> const& rhs) const
        {
            return subrule_list<
                self_t,
                subrule_list<
                    subrule_parser<ID2, DefT2, ContextT2>,
                    nil_t> >(
                        *this,
                        subrule_list<
                            subrule_parser<ID2, DefT2, ContextT2>, nil_t>(
                                rhs, nil_t()));
        }

        typename DefT::embed_t rhs;
        subrule_t const& start;
    };

    ///////////////////////////////////////////////////////////////////////////
    //
    //  subrule class
    //
    ///////////////////////////////////////////////////////////////////////////
    template <int ID, typename ContextT>
    struct subrule
        : public parser<subrule<ID, ContextT> >
        , public ContextT::base_t
    {
        typedef subrule<ID, ContextT> self_t;
        typedef subrule<ID, ContextT> const&  embed_t;

        typedef typename ContextT::context_linker_t context_t;
        typedef typename context_t::attr_t attr_t;

        BOOST_STATIC_CONSTANT(int, id = ID);

        template <typename ScannerT>
        struct result
        {
            typedef typename
                impl::get_subrule_result<ID, ScannerT, attr_t>::type type;
        };

        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse_main(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            result_t result;
            impl::parse_subrule<result_t, ScannerT, ID>::
                do_(result, scan);
            return result;
        }

        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            typedef parser_scanner_linker<ScannerT> scanner_t;
            BOOST_SPIRIT_CONTEXT_PARSE(
                scan, *this, scanner_t, context_t, result_t);
        }

        template <typename DefT>
        subrule_parser<ID, DefT, ContextT>
        operator=(parser<DefT> const& rhs) const
        {
            return subrule_parser<ID, DefT, ContextT>(*this, rhs.derived());
        }

    private:

        //  assignment of subrules is not allowed. Use subrules
        //  with identical IDs if you want to have aliases.

    #if !defined(BOOST_MSVC) && (BOOST_MSVC > 1200)
        subrule& operator=(subrule const&);
    #endif
        template <int ID2, typename ContextT2>
        subrule& operator=(subrule<ID2, ContextT2> const&);
    };

}} // namespace boost::spirit

#endif

