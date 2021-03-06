/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
// Original file Copyright Crytek GMBH or its affiliates, used under license.

#ifndef CRYINCLUDE_CRYMOVIE_SCENENODE_H
#define CRYINCLUDE_CRYMOVIE_SCENENODE_H

#pragma once

#include <AzCore/std/containers/map.h>

#include "AnimNode.h"
#include "SoundTrack.h"
#include "SelectTrack.h"

class CGotoTrack;
class CAnimEntityNode;

class CAnimSceneNode
    : public CAnimNode
{
public:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // helper interface for a uniform interface to legacy and component entity cameras
    class ISceneCamera
    {
    public:
        virtual ~ISceneCamera() = default;

        virtual const Vec3& GetPosition() const = 0;
        virtual const Quat& GetRotation() const = 0;
        virtual void SetPosition(const Vec3& localPosition) = 0;
        virtual void SetRotation(const Quat& localRotation) = 0;

        virtual float GetFoV() const = 0;
        virtual float GetNearZ() const = 0;

        // includes check for changes
        virtual void SetNearZAndFOVIfChanged(float fov, float nearZ) = 0;
        virtual void TransformPositionFromLocalToWorldSpace(Vec3& position) = 0;
        virtual void TransformPositionFromWorldToLocalSpace(Vec3& position) = 0;
        virtual void TransformRotationFromLocalToWorldSpace(Quat& rotation) = 0;
        // keeps existing world position
        virtual void SetWorldRotation(const Quat& rotation) = 0;

        // returns true if the camera has a parent
        virtual bool HasParent() const = 0;

    protected:
        ISceneCamera() {};
    };

    CAnimSceneNode(const int id);
    ~CAnimSceneNode();
    static void Initialize();

    virtual EAnimNodeType GetType() const { return eAnimNodeType_Director; }

    //////////////////////////////////////////////////////////////////////////
    // Overrides from CAnimNode
    //////////////////////////////////////////////////////////////////////////
    void Animate(SAnimContext& ec);
    void CreateDefaultTracks();

    bool GetCameraBoneLinkQuatT(IEntity* pEntity, QuatT& xform, bool bForceAnimationUpdate);

    virtual void Serialize(XmlNodeRef& xmlNode, bool bLoading, bool bLoadEmptyTracks);

    virtual void Activate(bool bActivate);

    // ovverided from IAnimNode/CAnimNode
    void OnStart() override;
    void OnReset() override;
    void OnPause() override;
    void OnStop() override;
    void OnLoop() override;

    //////////////////////////////////////////////////////////////////////////
    virtual unsigned int GetParamCount() const;
    virtual CAnimParamType GetParamType(unsigned int nIndex) const;

    void GetMemoryUsage(ICrySizer* pSizer) const
    {
        pSizer->AddObject(this, sizeof(*this));
        pSizer->AddObject(m_SoundInfo);
        CAnimNode::GetMemoryUsage(pSizer);
    }

    virtual void PrecacheStatic(float startTime) override;
    virtual void PrecacheDynamic(float time) override;

protected:
    virtual bool GetParamInfoFromType(const CAnimParamType& paramId, SParamInfo& info) const;

    void ResetSounds() override;
    void ReleaseSounds();   // Stops audio

private:
    void ApplyCameraKey(ISelectKey& key, SAnimContext& ec);
    void ApplyEventKey(IEventKey& key, SAnimContext& ec);
    void ApplyConsoleKey(IConsoleKey& key, SAnimContext& ec);
    void ApplyAudioKey(char const* const sTriggerName, bool const bPlay = true) override;
    void ApplySequenceKey(IAnimTrack* pTrack, int nPrevKey, int nCurrKey, ISequenceKey& key, SAnimContext& ec);
    void ApplyMusicKey(IMusicKey& key, SAnimContext& ec);

    void ApplyGotoKey(CGotoTrack*   poGotoTrack, SAnimContext& ec);

    bool GetEntityTransform(IAnimSequence* pSequence, IEntity* pEntity, float time, Vec3& vCamPos, Quat& qCamRot);
    bool GetEntityTransform(IEntity* pEntity, float time, Vec3& vCamPos, Quat& qCamRot);

    // fill retInterpolatedCameraParams with interpolated camera data. If firstCameraId is a valid AZ::EntityId, it is used. Otherwise it falls back to legacyFirstCamera.
    // should be non-null. Preference will be given to firstCamera if they are both non-null
    void InterpolateCameras(SCameraParams& retInterpolatedCameraParams, ISceneCamera* firstCamera, IAnimNode* firstCameraAnimNode,
        ISelectKey& firstKey, ISelectKey& secondKey, float time);

    virtual void InitializeTrackDefaultValue(IAnimTrack* pTrack, const CAnimParamType& paramType) override;

    // Cached parameters of node at given time.
    float m_time;

    IMovieSystem* m_pMovie;

    CSelectTrack* m_CurrentSelectTrack;
    int m_CurrentSelectTrackKeyNumber;
    IAnimNode* m_pCamNodeOnHoldForInterp;
    float m_lastPrecachePoint;

    //! Last animated key in track.
    int m_lastCameraKey;
    int m_lastEventKey;
    int m_lastConsoleKey;
    int m_lastMusicKey;
    int m_lastSequenceKey;
    int m_nLastGotoKey;
    int m_lastCaptureKey;
    bool m_bLastCapturingEnded;

    // used for legacy cameras only for clearing trigger area flags in OnReset;
    EntityId m_legacyCurrentCameraEntityId;

    struct InterpolatingCameraStartState
    {
        Vec3 m_interpolatedCamFirstPos;
        Quat m_interpolatedCamFirstRot;
        float m_FoV;
        float m_nearZ;
    };

    using keyIdx = int;

    // each camera key with a blend time > 0 needs a stashed initial xform for interpolation
    AZStd::map<keyIdx, InterpolatingCameraStartState>    m_InterpolatingCameraStartStates;

    std::vector<SSoundInfo> m_SoundInfo;

    ICVar* m_cvar_t_FixedStep;
};

#endif // CRYINCLUDE_CRYMOVIE_SCENENODE_H
