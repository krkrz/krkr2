#include "testEnv.hpp"

namespace {

class Creature {
    int health;
public:
    enum {MaxHealth = 100};
    Creature() : health(MaxHealth) {}
    int GetMaxHealth(void) {
        return MaxHealth;
    }
    int GetHealth(void) {
        return health;
    }
    void SetHealth(int newHealth) {
        health = newHealth;
    }
    static const SQChar *sqtypeof;
};

#ifdef SQPLUS_ENABLE_TYPEOF
const SQChar *Creature::sqtypeof = _T("Creature");
#else
const SQChar *Creature::sqtypeof = _T("instance");
#endif
}
DECLARE_INSTANCE_TYPE(Creature);


    
SQPLUS_TEST(Test_PassByReference)
{
    SQPLUS_TEST_TRACE();

    using namespace SqPlus;
    
    // === BEGIN example from forum post ===

    SQClassDef<Creature>(_T("Creature"))
        .func(&Creature::GetMaxHealth, _T("GetMaxHealth"))
        .func(&Creature::GetHealth, _T("GetHealth"))
        .func(&Creature::SetHealth, _T("SetHealth"))
        .staticVar(&Creature::sqtypeof, _T("sqtypeof"), VAR_ACCESS_READ_ONLY)
        ;
    Creature frodo;
    frodo.SetHealth(frodo.GetMaxHealth() / 2);

    RUN_SCRIPT(_T("function HealthPotionUse(Target) { \n\
                     local curHealth = Target.GetHealth(); \n\
                     local maxHealth = Target.GetMaxHealth(); \n\
                     if ((maxHealth - curHealth) > 15) { \n\
                       curHealth += 15; \n\
                     } else { \n\
                       curHealth = maxHealth; \n\
                     } \n\
                     Target.SetHealth(curHealth); \n\
                     print(typeof Target); \n \
                     assert(typeof(Target) == Target.sqtypeof); \n \
                     return Target; \n\
                   }"));

    
    // Pass by value and return a copy
    // (Must return by reference due to template system design).
    Creature newFrodo =
        SquirrelFunction<Creature&>(_T("HealthPotionUse"))(frodo);

    // Pass the address to directly modify frodo.
    SquirrelFunction<void>(_T("HealthPotionUse"))(&frodo);
    
    scprintf(_T("Frodo's health: %d %d\n"),
             frodo.GetHealth(), newFrodo.GetHealth());
    
    CHECK_EQUAL(frodo.GetHealth(), newFrodo.GetHealth());

    // === END example from forum post ===
}
