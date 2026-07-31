#include "ZPlayerExplode.h"
#include "Helpers/PlayerUtils.h"

void ZPlayerExplode::Start()
{
    SMatrix s_PlayerTransform;
    if (Utils::GetPlayerTransform(s_PlayerTransform)) {
		s_PlayerTransform.Trans.z += 0.25f; // offset explosion a bit upwards so it doesn't clip into the ground
        const ZExplosionEffectBase::SExplosionParams s_Params{
            .m_Position = s_PlayerTransform,
            .m_fTargetStrength = 10.0f,
        };

        ZExplosionEffectBase::SpawnExplosion(s_Params);
    }
}