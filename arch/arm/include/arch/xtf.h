/**
 * @file arch/arm/include/arch/xtf.h
 */
#ifndef XTF_ARM_XTF_H
#define XTF_ARM_XTF_H

struct feature_flags
{
    bool isinitdomain; /* Are we initial domain (dom0)? */
    bool isdirectmap;  /* Is memory directly mapped? */
};

extern struct feature_flags xtf_features;

#define use_hardware_layout() (xtf_features.isinitdomain || \
                               xtf_features.isdirectmap)

#endif /* XTF_ARM_XTF_H */

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
