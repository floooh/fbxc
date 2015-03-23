//------------------------------------------------------------------------------
//  ProxyBuilder.cc
//------------------------------------------------------------------------------
#include "ProxyBuilder.h"
#include "Log.h"

namespace FBXC {

//------------------------------------------------------------------------------
void
ProxyBuilder::Build(FbxScene* fbxScene, const std::string& fbxPath, ProxyScene& outProxyScene) {
    assert(fbxScene);
    
    outProxyScene.Object = fbxScene;
    outProxyScene.Properties.Add("file", fbxPath);
    BuildMetaData(fbxScene, outProxyScene);
    BuildTextures(fbxScene, outProxyScene);
    BuildMaterials(fbxScene, outProxyScene);
    BuildMeshes(fbxScene, outProxyScene);
}

//------------------------------------------------------------------------------
void
ProxyBuilder::BuildMetaData(FbxScene* fbxScene, ProxyScene& scene) {
    FbxDocumentInfo* info = fbxScene->GetSceneInfo();
    if (!info) {
        Log::Fatal("failed to get scene info from FBX scene\n");
    }
    scene.Properties.Add("title", info->mTitle);
    scene.Properties.Add("subject", info->mSubject);
    scene.Properties.Add("author", info->mAuthor);
    scene.Properties.Add("keywords", info->mKeywords);
    scene.Properties.Add("revision", info->mRevision);
    scene.Properties.Add("comment", info->mComment);
}

//------------------------------------------------------------------------------
void
ProxyBuilder::BuildUserProperties(FbxObject* fbxObject, ProxyObject& obj) {
    FbxProperty fbxProp = fbxObject->GetFirstProperty();
    while (fbxProp.IsValid()) {
        if (fbxProp.GetFlag(FbxPropertyFlags::eUserDefined)) {
            switch (fbxProp.GetPropertyDataType().GetType()) {
                case EFbxType::eFbxChar:
                case EFbxType::eFbxUChar:
                case EFbxType::eFbxShort:
                case EFbxType::eFbxUShort:
                case EFbxType::eFbxInt:
                case EFbxType::eFbxUInt:
                case EFbxType::eFbxEnum:
                    obj.UserProperties.Add(fbxProp.GetName().Buffer(), fbxProp.Get<int>());
                    break;
                case EFbxType::eFbxBool:
                    obj.UserProperties.Add(fbxProp.GetName().Buffer(), fbxProp.Get<bool>());
                    break;
                case EFbxType::eFbxHalfFloat:
                case EFbxType::eFbxFloat:
                case EFbxType::eFbxDouble:
                    obj.UserProperties.Add(fbxProp.GetName().Buffer(), fbxProp.Get<double>());
                    break;
                case EFbxType::eFbxDouble2:
                    obj.UserProperties.Add(fbxProp.GetName().Buffer(), fbxProp.Get<FbxDouble2>());
                    break;
                case EFbxType::eFbxDouble3:
                    obj.UserProperties.Add(fbxProp.GetName().Buffer(), fbxProp.Get<FbxDouble3>());
                    break;
                case EFbxType::eFbxDouble4:
                    obj.UserProperties.Add(fbxProp.GetName().Buffer(), fbxProp.Get<FbxDouble4>());
                    break;
                case EFbxType::eFbxString:
                    obj.UserProperties.Add(fbxProp.GetName().Buffer(), fbxProp.Get<FbxString>());
                    break;
                default:
                    break;
            }
        }
        fbxProp = fbxObject->GetNextProperty(fbxProp);
    }
}

//------------------------------------------------------------------------------
void
ProxyBuilder::BuildTextures(FbxScene* fbxScene, ProxyScene& scene) {
    const int numTextures = fbxScene->GetTextureCount();
    for (int texIndex = 0; texIndex < numTextures; texIndex++) {
        FbxTexture* fbxTex = fbxScene->GetTexture(texIndex);
    
        scene.Textures.emplace_back();
        ProxyObject& tex = scene.Textures.back();
        tex.Object = fbxTex;
        tex.Properties.Add("name", fbxTex->GetName());
        tex.Properties.Add("id", fbxTex->GetUniqueID());

        if (fbxTex->GetClassId().Is(FbxFileTexture::ClassId)) {
            FbxFileTexture* fbxFileTex = (FbxFileTexture*) fbxTex;
            tex.Properties.Add("type", "file");
            tex.Properties.Add("usematerial", fbxFileTex->UseMaterial.Get());
            tex.Properties.Add("usemipmap", fbxFileTex->UseMipMap.Get());
            tex.Properties.Add("filename", fbxFileTex->GetRelativeFileName());
            static std::map<FbxFileTexture::EMaterialUse, const char*> matUseToStr = {
                { FbxFileTexture::eModelMaterial, "model" },
                { FbxFileTexture::eDefaultMaterial, "default" }
            };
            const FbxFileTexture::EMaterialUse matUse = fbxFileTex->GetMaterialUse();
            const char* matUseStr = "invalid";
            if (matUseToStr.find(matUse) != matUseToStr.end()) {
                matUseStr = matUseToStr[matUse];
            }
            tex.Properties.Add("materialuse", matUseStr);
        }
        else {
            tex.Properties.Add("type", "procedural");
        }
        tex.Properties.Add("swapuv", fbxTex->GetSwapUV());
        tex.Properties.Add("premultiplyalpha", fbxTex->GetPremultiplyAlpha());
        static std::map<FbxTexture::EAlphaSource, const char*> alphaSourceToStr = {
            { FbxTexture::eNone, "none" },
            { FbxTexture::eRGBIntensity, "rgbintensity" },
            { FbxTexture::eBlack, "black" }
        };
        const FbxTexture::EAlphaSource alphaSource = fbxTex->GetAlphaSource();
        const char* alphaSourceStr = "invalid";
        if (alphaSourceToStr.find(alphaSource) != alphaSourceToStr.end()) {
            alphaSourceStr = alphaSourceToStr[alphaSource];
        }
        tex.Properties.Add("alphasource", alphaSourceStr);
        tex.Properties.Add("croppingleft", fbxTex->GetCroppingLeft());
        tex.Properties.Add("croppingtop", fbxTex->GetCroppingTop());
        tex.Properties.Add("croppingright", fbxTex->GetCroppingRight());
        tex.Properties.Add("croppingbottom", fbxTex->GetCroppingBottom());
        static std::map<FbxTexture::EMappingType, const char*> mappingTypeToStr = {
            { FbxTexture::eNull, "null" },
            { FbxTexture::ePlanar, "planar" },
            { FbxTexture::eSpherical, "spherical" },
            { FbxTexture::eCylindrical, "cylindrical" },
            { FbxTexture::eBox, "box" },
            { FbxTexture::eFace, "face" },
            { FbxTexture::eUV, "uv" },
            { FbxTexture::eEnvironment, "environment" }
        };
        const FbxTexture::EMappingType mappingType = fbxTex->GetMappingType();
        const char* mappingTypeStr = "invalid";
        if (mappingTypeToStr.find(mappingType) != mappingTypeToStr.end()) {
            mappingTypeStr = mappingTypeToStr[mappingType];
        }
        tex.Properties.Add("mappingtype", mappingTypeStr);
        static std::map<FbxTexture::EPlanarMappingNormal, const char*> pmnToStr = {
            { FbxTexture::ePlanarNormalX, "x" },
            { FbxTexture::ePlanarNormalY, "y" },
            { FbxTexture::ePlanarNormalZ, "z" }
        };
        const FbxTexture::EPlanarMappingNormal pmn = fbxTex->GetPlanarMappingNormal();
        const char* pmnStr = "invalid";
        if (pmnToStr.find(pmn) != pmnToStr.end()) {
            pmnStr = pmnToStr[pmn];
        }
        tex.Properties.Add("planarmappingnormal", pmnStr);
        static std::map<FbxTexture::ETextureUse, const char*> texUseToStr = {
            { FbxTexture::eStandard, "standard" },
            { FbxTexture::eShadowMap, "shadowmap" },
            { FbxTexture::eLightMap, "lightmap" },
            { FbxTexture::eSphericalReflectionMap, "sphericalreflectionmap" },
            { FbxTexture::eSphereReflectionMap, "spherereflectionmap" },
            { FbxTexture::eBumpNormalMap, "bumpnormalmap" }
        };
        const FbxTexture::ETextureUse texUse = fbxTex->GetTextureUse();
        const char* texUseStr = "invalid";
        if (texUseToStr.find(texUse) != texUseToStr.end()) {
            texUseStr = texUseToStr[texUse];
        }
        tex.Properties.Add("textureuse", texUseStr);
        static std::map<FbxTexture::EWrapMode, const char*> wrapModeToStr = {
            { FbxTexture::eRepeat, "repeat" },
            { FbxTexture::eClamp, "clamp" }
        };
        const FbxTexture::EWrapMode wrapModeU = fbxTex->GetWrapModeU();
        const FbxTexture::EWrapMode wrapModeV = fbxTex->GetWrapModeV();
        const char* wrapModeUStr = "invalid";
        const char* wrapModeVStr = "invalid";
        if (wrapModeToStr.find(wrapModeU) != wrapModeToStr.end()) {
            wrapModeUStr = wrapModeToStr[wrapModeU];
        }
        if (wrapModeToStr.find(wrapModeV) != wrapModeToStr.end()) {
            wrapModeVStr = wrapModeToStr[wrapModeV];
        }
        tex.Properties.Add("wrapmodeu", wrapModeUStr);
        tex.Properties.Add("wrapmodev", wrapModeVStr);
        static std::map<FbxTexture::EBlendMode, const char*> blendModeToStr = {
            { FbxTexture::eTranslucent, "translucent" },
            { FbxTexture::eAdditive, "additive" },
            { FbxTexture::eModulate, "modulate" },
            { FbxTexture::eModulate2, "modulate2" },
            { FbxTexture::eOver, "over" }
        };
        const FbxTexture::EBlendMode blendMode = fbxTex->GetBlendMode();
        const char* blendModeStr = "invalid";
        if (blendModeToStr.find(blendMode) != blendModeToStr.end()) {
            blendModeStr = blendModeToStr[blendMode];
        };
        tex.Properties.Add("blendmode", blendModeStr);
        tex.Properties.Add("alpha", fbxTex->Alpha.Get());
        tex.Properties.Add("translation", fbxTex->Translation.Get());
        tex.Properties.Add("rotation", fbxTex->Rotation.Get());
        tex.Properties.Add("scaling", fbxTex->Scaling.Get());
        tex.Properties.Add("rotationpivot", fbxTex->RotationPivot.Get());
        tex.Properties.Add("scalingpivot", fbxTex->ScalingPivot.Get());
        tex.Properties.Add("uvset", fbxTex->UVSet.Get());
        
        BuildUserProperties(fbxTex, tex);
    }
}

//------------------------------------------------------------------------------
bool
ProxyBuilder::BuildPropertyConnection(const FbxPropertyT<FbxDouble3>& prop, const FbxCriteria& criteria, const char* name, PropertyMap& props) {
    FbxObject* srcObj = prop.GetSrcObject(criteria);
    if (srcObj) {
        props.Add(name, srcObj->GetName());
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
ProxyBuilder::BuildMaterials(FbxScene* fbxScene, ProxyScene& scene) {

    // property connection search criteria for connected textures
    FbxCriteria texCriteria = FbxCriteria::ObjectType(FbxTexture::ClassId);

    const int numMaterials = fbxScene->GetMaterialCount();
    for (int matIndex = 0; matIndex < numMaterials; matIndex++) {
        FbxSurfaceMaterial* fbxMat = fbxScene->GetMaterial(matIndex);

        scene.Materials.emplace_back();
        ProxyObject& mat = scene.Materials.back();
        mat.Object = fbxMat;
        mat.Properties.Add("name", fbxMat->GetName());
        mat.Properties.Add("id", fbxMat->GetUniqueID());
        mat.Properties.Add("shadingmodel", fbxMat->ShadingModel.Get().Lower());
        mat.Properties.Add("multilayer", fbxMat->MultiLayer.Get());
        
        // hw shader material?
        const FbxImplementation* impl = GetImplementation(fbxMat, FBXSDK_IMPLEMENTATION_HLSL);
        const char* hwShaderType = nullptr;
        if (impl) {
            hwShaderType = "HLSL";
        }
        else {
            impl = GetImplementation(fbxMat, FBXSDK_IMPLEMENTATION_CGFX);
            if (impl) {
                hwShaderType = "CGFX";
            }
        }
        if (impl) {
            mat.Properties.Add("hwshadertype", hwShaderType);
            // FIXME: add iterate over shader attributes
        }
        else {
            if (fbxMat->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
                const FbxSurfaceLambert* lamb = (FbxSurfaceLambert*) fbxMat;
                
                mat.Properties.Add("emissive", lamb->Emissive.Get());
                mat.Properties.Add("emissivefactor", lamb->EmissiveFactor.Get());
                mat.Properties.Add("ambient", lamb->Ambient.Get());
                mat.Properties.Add("ambientfactor", lamb->AmbientFactor.Get());
                mat.Properties.Add("diffuse", lamb->Diffuse.Get());
                mat.Properties.Add("diffusefactor", lamb->DiffuseFactor.Get());
                mat.Properties.Add("normalmap", lamb->NormalMap.Get());
                mat.Properties.Add("bump", lamb->Bump.Get());
                mat.Properties.Add("bumpfactor", lamb->BumpFactor.Get());
                mat.Properties.Add("transparentcolor", lamb->TransparentColor.Get());
                mat.Properties.Add("transparencyfactor", lamb->TransparencyFactor.Get());
                mat.Properties.Add("displacementcolor", lamb->DisplacementColor.Get());
                mat.Properties.Add("displacementfactor", lamb->DisplacementFactor.Get());
                mat.Properties.Add("vectordisplacementcolor", lamb->VectorDisplacementColor.Get());
                mat.Properties.Add("vectordisplacementfactor", lamb->VectorDisplacementFactor.Get());
                
                // texture connections
                BuildPropertyConnection(lamb->Emissive, texCriteria, "emissive_texture", mat.Properties);
                BuildPropertyConnection(lamb->Ambient, texCriteria, "ambient_texture", mat.Properties);
                BuildPropertyConnection(lamb->Diffuse, texCriteria, "diffuse_texture", mat.Properties);
                BuildPropertyConnection(lamb->NormalMap, texCriteria, "normalmap_texture", mat.Properties);
                BuildPropertyConnection(lamb->Bump, texCriteria, "bump_texture", mat.Properties);
                BuildPropertyConnection(lamb->TransparentColor, texCriteria, "transparentcolor_texture", mat.Properties);
                BuildPropertyConnection(lamb->DisplacementColor, texCriteria, "displacementcolor_texture", mat.Properties);
                BuildPropertyConnection(lamb->VectorDisplacementColor, texCriteria, "vectordisplacementcolor_texture", mat.Properties);
            }
            if (fbxMat->GetClassId().Is(FbxSurfacePhong::ClassId)) {
                const FbxSurfacePhong* phong = (FbxSurfacePhong*) fbxMat;
                
                mat.Properties.Add("specular", phong->Specular.Get());
                mat.Properties.Add("specularfactor", phong->SpecularFactor.Get());
                mat.Properties.Add("shininess", phong->Shininess.Get());
                mat.Properties.Add("reflection", phong->Reflection.Get());
                mat.Properties.Add("reflectionfactor", phong->ReflectionFactor.Get());
                
                // texture connections
                BuildPropertyConnection(phong->Specular, texCriteria, "specular_texture", mat.Properties);
                BuildPropertyConnection(phong->Shininess, texCriteria, "shininess_texture", mat.Properties);
                BuildPropertyConnection(phong->Reflection, texCriteria, "reflection_texture", mat.Properties);
            }
        }
        BuildUserProperties(fbxMat, mat);
    }
}

//------------------------------------------------------------------------------
void
ProxyBuilder::BuildMeshes(FbxScene* fbxScene, ProxyScene& scene) {

    const int numGeoms = fbxScene->GetGeometryCount();
    for (int geomIndex = 0; geomIndex < numGeoms; geomIndex++) {
        FbxGeometry* fbxGeom = fbxScene->GetGeometry(geomIndex);
        
        // only look at meshes
        if (fbxGeom->GetClassId().Is(FbxMesh::ClassId)) {
            FbxMesh* fbxMesh = (FbxMesh*) fbxGeom;
            
            scene.Meshes.emplace_back();
            ProxyObject& mesh = scene.Meshes.back();
            
            // NOTE: meshes don't have names, so use unique id as identifier
            mesh.Properties.Add("id", fbxMesh->GetUniqueID());
            mesh.Properties.Add("numpoints", fbxMesh->GetControlPointsCount());
            mesh.Properties.Add("numpolygons", fbxMesh->GetPolygonCount());
            
            assert(fbxMesh->GetLayerCount() > 0);
            FbxLayer* fbxLayer = fbxMesh->GetLayer(0);
            mesh.Properties.Add("hasnormals", fbxLayer->GetNormals() != nullptr);
            mesh.Properties.Add("hastangents", fbxLayer->GetTangents() != nullptr);
            mesh.Properties.Add("hasbinormals", fbxLayer->GetBinormals() != nullptr);
            mesh.Properties.Add("hasmaterials", fbxLayer->GetMaterials() != nullptr);
            mesh.Properties.Add("haspolygongroups", fbxLayer->GetPolygonGroups() != nullptr);
            mesh.Properties.Add("hasvertexcolor", fbxLayer->GetVertexColors() != nullptr);
            mesh.Properties.Add("hasuserdata", fbxLayer->GetUserData() != nullptr);
            mesh.Properties.Add("hasvisibility", fbxLayer->GetVisibility() != nullptr);
            if (fbxLayer->GetUVSetCount() > 0) {
                std::vector<Value> uvSets;
                FbxArray<const FbxLayerElementUV*> fbxUvSets = fbxLayer->GetUVSets();
                for (int i = 0; i < fbxUvSets.Size(); i++) {
                    Value val;
                    val.Set(fbxUvSets[i]->GetName());
                    uvSets.push_back(val);
                }
                mesh.Properties.Add("uvsets", uvSets);
            }
            BuildUserProperties(fbxMesh, mesh);
        }
    }
}

} // namespace FBXC