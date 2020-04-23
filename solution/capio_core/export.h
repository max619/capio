#pragma once

#ifdef CAPIO_DYNAMIC
#ifdef _CAPIO_EXPORT
#define CAPIO_EXPORT __declspec(dllexport)
#else
#define CAPIO_EXPORT __declspec(dllimport)
#endif
#define CAPIO_EXPORT_CLASS_TEMPLATE(t)	template class CAPIO_EXPORT t
#define CAPIO_EXPORT_SMART_POINTERS(t)	template class CAPIO_EXPORT std::unique_ptr<t, std::default_delete<t>>;\
										template class CAPIO_EXPORT std::unique_ptr<t>;\

#else
#define CAPIO_EXPORT
#define CAPIO_EXPORT_CLASS_TEMPLATE(t)
#endif