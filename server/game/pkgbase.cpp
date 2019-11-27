#include "pkgbase.h"
#include "role.h"
CPkgBase::CPkgBase(CRole* role, std::string* pkg)
	:m_role(role)
{
	role->RegPkg(this,pkg);
}
