// The MIT License (MIT)

// Copyright (c) 2016, Microsoft

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include <stddef.h>     // size_t, ptrdiff_t parameters.

#include "NativeJIT/CodeGen/FunctionBuffer.h"   // FunctionBuffer embedded.
#include "NativeJIT/Function.h"                 // Function in typedef.


namespace NativeJIT
{
    class Allocator;
    class ExecutionBuffer;
    class FunctionBuffer;
};

using namespace NativeJIT;

namespace BitFunnel
{
    class CompileNode;

    //*************************************************************************
    //
    // MatchTreeCompiler
    //
    //*************************************************************************
    class MatchTreeCompiler
    {
    public:
        MatchTreeCompiler(ExecutionBuffer & codeAllocator,
                          Allocator & allocator,
                          CompileNode const & tree);

        size_t Run(size_t slicecount,
                   char * const * slicebuffers,
                   size_t iterationsperslice,
                   ptrdiff_t const * rowoffsets);

    private:
        friend class MatcherNode;

        static size_t CallbackHelper(/*MatchTreeCompiler& node, */size_t value);

        //        typedef void(*Callback)(MatchTreeCompiler& node, size_t value);
        typedef size_t(*Callback)(size_t value);

        struct Parameters
        {
        public:
            size_t m_sliceCount;
            char * const * m_sliceBuffers;
            size_t m_iterationsPerSlice;
            ptrdiff_t const * m_rowOffsets;
            Callback m_callback;
        };
        static_assert(std::is_standard_layout<Parameters>::value,
                      "Parameters must be standard layout.");

        FunctionBuffer m_code;

        typedef Function<size_t, Parameters const *> Prototype;
        Prototype::FunctionType m_function;
    };


    //*************************************************************************
    //
    // MatcherNode
    //
    //*************************************************************************
    class MatcherNode : public NativeJIT::Node<size_t>
    {
    public:
        MatcherNode(MatchTreeCompiler::Prototype& expression,
                    CompileNode const & matchTree);

        virtual ExpressionTree::Storage<size_t> CodeGenValue(ExpressionTree& tree) override;

        virtual void Print(std::ostream& out) const override;

    private:
        void EmitRegisterInitialization(ExpressionTree& tree);
        void EmitOuterLoop(ExpressionTree& tree);
        void EmitInnerLoop(ExpressionTree& tree);
        void EmitFinishIteration(ExpressionTree& tree);

        CompileNode const & m_matchTree;

        Register<8u, false> m_param1;
        Register<8u, false> m_return;

        Storage<size_t> m_sliceCount;
        Storage<char * const *> m_sliceBuffers;
        Storage<size_t> m_iterationsPerSlice;
        Storage<ptrdiff_t const *> m_rowOffsets;
        Storage<MatchTreeCompiler::Callback> m_callback;
        Storage<size_t> m_innerLoopLimit;
        Storage<size_t> m_matchFound;
        Storage<size_t> m_temp;
    };
}
