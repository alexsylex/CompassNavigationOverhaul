#pragma once

template <typename T>
consteval auto type_name() -> std::string_view
{
	std::string_view funcsig = __FUNCSIG__;

	size_t startpos = funcsig.find(" type_name<") + 1;
	size_t endpos = funcsig.rfind(">(void)");
	size_t len = endpos - startpos;

	return funcsig.substr(startpos, len);
}

template <typename T>
consteval auto unscoped_type_name() -> std::string_view
{
	std::string_view fullTypeName = type_name<T>();

	size_t scopepos = fullTypeName.rfind("::") + 1;
	size_t startpos = (scopepos != fullTypeName.npos) ? scopepos + 1 : 0;

	return fullTypeName.substr(startpos);
}