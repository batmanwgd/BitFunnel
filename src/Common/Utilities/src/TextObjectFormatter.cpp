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

#include <iomanip>
#include <limits>

#include "BitFunnel/Utilities/IPersistableObject.h"
#include "BitFunnel/Utilities/TextObjectFormatter.h"
#include "LoggerInterfaces/Logging.h"

namespace BitFunnel
{
    namespace Factories
    {
        IObjectFormatter* CreateObjectFormatter(std::ostream& output)
        {
            return new TextObjectFormatter(output);
        }
    }

    TextObjectFormatter::TextObjectFormatter(std::ostream& output)
        : m_output(output),
          m_indentation(0)
    {
    }


    TextObjectFormatter::TextObjectFormatter(std::ostream& output, unsigned indentation)
        : m_output(output),
          m_indentation(indentation)
    {
    }


    void TextObjectFormatter::OpenObject(const IPersistableObject& object)
    {
        m_output << object.GetTypeName() << " {" << std::endl;
        ++m_indentation;
        m_objectFields.push(0);
    }


    void TextObjectFormatter::OpenObjectField(const char* name)
    {
        if (m_objectFields.top() > 0)
        {
            m_output << "," << std::endl;
        }
        m_objectFields.top()++;
        Indent();
        m_output << name << ": ";
    }


    void TextObjectFormatter::CloseObject()
    {
        --m_indentation;
        m_objectFields.pop();
        m_output << std::endl;
        Indent();
        m_output << "}";
    }


    void TextObjectFormatter::NullObject()
    {
        m_output << "";
    }


    void TextObjectFormatter::OpenList()
    {
        m_output << "[";
        ++m_indentation;
        m_listItems.push(0);
    }


    void TextObjectFormatter::OpenListItem()
    {
        if (m_listItems.top() > 0)
        {
            m_output << ",";
        }
        m_output << std::endl;
        m_listItems.top()++;
        Indent();
    }


    void TextObjectFormatter::CloseList()
    {
        m_output << std::endl;
        --m_indentation;
        m_listItems.pop();
        Indent();
        m_output << "]";
    }


    void TextObjectFormatter::OpenPrimitive(const char* name)
    {
        m_output << name << "(";
        m_primitiveItems = 0;
    }


    void TextObjectFormatter::OpenPrimitiveItem()
    {
        if (m_primitiveItems > 0)
        {
            m_output << ", ";
        }
        m_primitiveItems++;
    }


    void TextObjectFormatter::ClosePrimitive()
    {
        m_output << ")";
    }


    // TODO: Consider using template-specialized helper function.
    void TextObjectFormatter::Format(bool value)
    {
        if (value)
        {
            m_output << "true";
        }
        else
        {
            m_output << "false";
        }
    }


    void TextObjectFormatter::Format(int value)
    {
        m_output << value;
    }


    void TextObjectFormatter::Format(unsigned value)
    {
        m_output << value;
    }


    void TextObjectFormatter::Format(size_t value)
    {
        m_output << value;
    }


    void TextObjectFormatter::Format(double value)
    {
        m_output << value;
    }


    void TextObjectFormatter::Format(const char* value)
    {
        m_output << value;
    }


    void TextObjectFormatter::FormatStringLiteral(const char* value)
    {
        m_output << '"';

        for (const char* current = value; *current != '\0'; ++current)
        {
            if (*current == '\\' || *current == '"')
            {
                m_output << '\\';
            }
            m_output << *current;
        }

        m_output << '"';
    }


    void TextObjectFormatter::Indent()
    {
        // TODO: m_indentation * 2 could theoretically overflow an unsigned.
        LogAssertB(m_indentation * 2 < static_cast<unsigned int>(std::numeric_limits<int>::max()),
                   "m_indentation overflow.");
        int actualIndentation = static_cast<int>(m_indentation) * 2;
        m_output << std::setfill(' ') <<
            std::setw(actualIndentation) << "";
    }
}
