/******************************************************************************
 *
 * 	Effects.h by z.keyring (c) 2013 All Rights Reserved.
 *
 * 					--code with gVim :)
 *****************************************************************************/

#ifndef __EFFECTS_H__
#define __EFFECTS_H__ 1

#include "AppUtil.h"

/*---- Effect ----*/

class Effect{

	public:
		Effect(ID3D11Device *device, const std::wstring &filename);
		virtual ~Effect();

	private:
		Effect(const Effect& rhs);
		Effect& operator=(const Effect& rhs);

	protected:
		ID3DX11Effect *mFX;
};



/*---- BasicEffect ----*/

class BasicEffect : public Effect{

	public:
		BasicEffect(ID3D11Device *device, const std::wstring &filename);
		~BasicEffect();

		void setWorldViewProjMat(CXMMATRIX M){

			worldViewProjMat->SetMatrix(reinterpret_cast<const float*>(&M));
		}

		void setWorldMat(CXMMATRIX M){

			worldMat->SetMatrix(reinterpret_cast<const float*>(&M));
		}

		void setWorldInvTransposeMat(CXMMATRIX M){

			worldInvTransposeMat->SetMatrix(reinterpret_cast<const float*>(&M));
		}

		void setTexTransformMat(CXMMATRIX M){

			texTransformMat->SetMatrix(reinterpret_cast<const float*>(&M));
		}

		void setEyePosVec(const XMFLOAT3 &v){

			eyePosVec->SetRawValue(&v, 0, sizeof(XMFLOAT3));
		}

		void setDirLights(const DirectLight *lights){

			dirLights->SetRawValue(lights, 0, 3 * sizeof(DirectLight));
		}

		void setMaterial(const Material &mater){

			material->SetRawValue(&mater, 0, sizeof(Material));
		}

		void setDiffuseMap(ID3D11ShaderResourceView *tex){

			diffuseMap->SetResource(tex);
		}

		void setCubeMap(ID3D11ShaderResourceView *tex){

			cubeMap->SetResource(tex);
		}

		ID3DX11EffectTechnique *Light1Tech;
		ID3DX11EffectTechnique *Light2Tech;
		ID3DX11EffectTechnique *Light3Tech;

		ID3DX11EffectMatrixVariable *worldViewProjMat;
		ID3DX11EffectMatrixVariable *worldMat;
		ID3DX11EffectMatrixVariable *worldInvTransposeMat;
		ID3DX11EffectMatrixVariable *texTransformMat;

		ID3DX11EffectVectorVariable *eyePosVec;

		ID3DX11EffectVariable *dirLights;
		ID3DX11EffectVariable *material;

		ID3DX11EffectShaderResourceVariable *diffuseMap;
		ID3DX11EffectShaderResourceVariable *cubeMap;
};


/*---- SkyEffect ----*/

class SkyEffect : public Effect{

	public:
		SkyEffect(ID3D11Device *device, const std::wstring &filename);
		~SkyEffect();

		void setWorldViewProjMat(CXMMATRIX M){

			worldViewProjMat->SetMatrix(reinterpret_cast<const float*>(&M));
		}

		void setCubeMap(ID3D11ShaderResourceView *cubemap){

			cubeMap->SetResource(cubemap);
		}

		ID3DX11EffectTechnique *SkyTech;

		ID3DX11EffectMatrixVariable *worldViewProjMat;

		ID3DX11EffectShaderResourceVariable *cubeMap;
};


/*---- GrassEffect ----*/

class GrassEffect : public Effect{

	public:
		GrassEffect(ID3D11Device *device, const std::wstring &filename);
		~GrassEffect();

		void setWorldViewProjMat(CXMMATRIX M){

			worldViewProjMat->SetMatrix(reinterpret_cast<const float*>(&M));
		}

		void setDiffuseMap(ID3D11ShaderResourceView *tex){

			diffuseMap->SetResource(tex);
		}

		void setTime(float dt){
			
			timeVar->SetFloat(dt);
		}

		ID3DX11EffectTechnique *GrassTech;

		ID3DX11EffectMatrixVariable *worldViewProjMat;

		ID3DX11EffectScalarVariable *timeVar;

		ID3DX11EffectShaderResourceVariable *diffuseMap;
};


/*---- Effects ----*/

class Effects{

	public:
		static BasicEffect *BasicFX;
		static SkyEffect *SkyFX;
		static GrassEffect *GrassFX;

		static void InitAll(ID3D11Device *device);
		static void DestroyAll();
};

#endif    //__EFFECTS_H__
