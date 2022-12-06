#include "runtime/function/framework/component/lua/lua_component.h"
#include "runtime/core/base/macro.h"
#include "runtime/function/framework/object/object.h"
namespace Piccolo
{
    bool find_component_field(std::weak_ptr<GObject>     game_object,
                              const char*                field_full_name,
                              Reflection::FieldAccessor& target_field,
                              void*&                     target_instance)
    {

        auto               components = game_object.lock()->getComponents();
        std::istringstream iss(field_full_name);
        std::string        curname;
        std::getline(iss, curname, '.');
        auto component_iter = std::find_if(
            components.begin(), components.end(), [curname](auto c) { return c.getTypeName() == curname; });
        if (component_iter != components.end())
        {
            auto meta = Piccolo::Reflection::TypeMeta::newMetaFromName(curname);

            void* field_instance = (*component_iter).getPtr();
            // find target field
            while (std::getline(iss, curname, '.'))
            {
                Reflection::FieldAccessor* fields;
                int                        fields_count = meta.getFieldsList(fields);
                auto                       field_iter   = std::find_if(
                    fields, fields + fields_count, [curname](auto f) { return f.getFieldName() == curname; });
                if (field_iter == fields + fields_count)
                {
                    delete[] fields;
                    return false;
                }

                target_field = *field_iter;
                delete[] fields;
                target_instance = field_instance;
                field_instance  = target_field.get(target_instance);

                target_field.getTypeMeta(meta);
            }
        }
        return true;
    }

    template<typename T>
    void LuaComponent::set(std::weak_ptr<GObject> game_object, const char* field_full_name, T val)
    {
        Reflection::FieldAccessor target_field;
        void*                     target_instance;
        if (find_component_field(game_object, field_full_name, target_field, target_instance))
        {

            target_field.set(target_instance, &val);
        }
    }
    template<typename T>
    T LuaComponent::get(std::weak_ptr<GObject> game_object, const char* field_full_name)
    {
        Reflection::FieldAccessor target_field;
        void*                     target_instance;
        if (find_component_field(game_object, field_full_name, target_field, target_instance))
        {
            return *(T*)target_field.get(target_instance);
        }
    }

    void LuaComponent::postLoadResource(std::weak_ptr<GObject> parent_object)
    {
        m_parent_object = parent_object;
        m_lua_state.open_libraries(sol::lib::base);
        m_lua_state.set_function("set_float", &LuaComponent::set<float>);
        m_lua_state.set_function("get_bool", &LuaComponent::get<bool>);
        m_lua_state["GameObject"] = m_parent_object;
        return;
    }

    void LuaComponent::tick(float delta_time)
    {
        // LOG_INFO(m_lua_script);
        m_lua_state.script(m_lua_script);
    }

} // namespace Piccolo
