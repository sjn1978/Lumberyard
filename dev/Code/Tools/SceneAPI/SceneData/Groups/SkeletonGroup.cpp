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

#include <AzCore/RTTI/ReflectContext.h>
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <SceneAPI/SceneCore/DataTypes/Rules/IRule.h>
#include <SceneAPI/SceneData/Groups/SkeletonGroup.h>
#include <SceneAPI/SceneData/GraphData/RootBoneData.h>

namespace AZ
{
    namespace SceneAPI
    {
        namespace SceneData
        {
            AZ_CLASS_ALLOCATOR_IMPL(SkeletonGroup, SystemAllocator, 0)

            const AZStd::string& SkeletonGroup::GetName() const
            {
                return m_name;
            }

            void SkeletonGroup::SetName(const AZStd::string& name)
            {
                m_name = name;
            }

            void SkeletonGroup::SetName(AZStd::string&& name)
            {
                m_name = AZStd::move(name);
            }

            Containers::RuleContainer& SkeletonGroup::GetRuleContainer()
            {
                return m_rules;
            }

            const Containers::RuleContainer& SkeletonGroup::GetRuleContainerConst() const
            {
                return m_rules;
            }
            
            const AZStd::string& SkeletonGroup::GetSelectedRootBone() const
            {
                return m_selectedRootBone;
            }

            void SkeletonGroup::SetSelectedRootBone(const AZStd::string& selectedRootBone)
            {
                m_selectedRootBone = selectedRootBone;
            }

            void SkeletonGroup::Reflect(ReflectContext* context)
            {
                SerializeContext* serializeContext = azrtti_cast<SerializeContext*>(context);
                if (!serializeContext)
                {
                    return;
                }

                serializeContext->Class<SkeletonGroup, DataTypes::ISkeletonGroup>()->Version(2, VersionConverter)
                    ->Field("name", &SkeletonGroup::m_name)
                    ->Field("selectedRootBone", &SkeletonGroup::m_selectedRootBone)
                    ->Field("rules", &SkeletonGroup::m_rules);

                EditContext* editContext = serializeContext->GetEditContext();
                if (editContext)
                {
                    editContext->Class<SkeletonGroup>("Skeleton group", "Name and configure a skeleton from your source file.")
                        ->ClassElement(Edit::ClassElements::EditorData, "")
                            ->Attribute("AutoExpand", true)
                            ->Attribute(Edit::Attributes::NameLabelOverride, "")
                        ->DataElement(AZ_CRC("ManifestName", 0x5215b349), &SkeletonGroup::m_name, "Name skeleton",
                            "Name the skeleton as you want it to appear in the Lumberyard Asset Browser.")
                            ->Attribute("FilterType", DataTypes::ISkeletonGroup::TYPEINFO_Uuid())
                        ->DataElement("NodeListSelection", &SkeletonGroup::m_selectedRootBone, "Select root bone", "Select the root bone of the skeleton.")
                            ->Attribute("ClassTypeIdFilter", AZ::SceneData::GraphData::RootBoneData::TYPEINFO_Uuid())
                        ->DataElement(Edit::UIHandlers::Default, &SkeletonGroup::m_rules, "", "Add or remove rules to fine-tune the export process.")
                            ->Attribute(AZ::Edit::Attributes::Visibility, AZ_CRC("PropertyVisibility_ShowChildrenOnly", 0xef428f20));
                }
            }


            bool SkeletonGroup::VersionConverter(SerializeContext& context, SerializeContext::DataElementNode& classElement)
            {
                const unsigned int version = classElement.GetVersion();

                // Replaced vector<IRule> with RuleContainer.
                if (version == 1)
                {
                    return Containers::RuleContainer::VectorToRuleContainerConverter(context, classElement);
                }

                return true;
            }

        } // namespace SceneData
    } // namespace SceneAPI
} // namespace AZ