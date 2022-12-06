#include "common/precompiled.h"

#include "class.h"
#include "method.h"



Method::Method(
    const Cursor &cursor, 
    const Namespace &currentNamespace, 
    Class *parent
)
    : TypeInfo( cursor, currentNamespace )
    , m_parent( parent )
    , m_name( cursor.getSpelling( ) )
{
}

bool Method::ShouldCompile(void) const
{
    return isAccessible( ); 
}


bool Method::isAccessible(void) const
{
    // if the parent wants white listed method, then we must have 
    // the enable flag
    if (m_parent->m_meta_data.getFlag(NativeProperty::WhiteListMethods))
    {
        return m_meta_data.getFlag(NativeProperty::Enable);

    }

    // must not be explicitly disabled
    return (m_parent->m_meta_data.getFlag(NativeProperty::Methods) || m_parent->m_meta_data.getFlag(NativeProperty::All))
        && !m_meta_data.getFlag(NativeProperty::Disable);
}
