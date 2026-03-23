/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "ExternalEventHelper.h"

#include "ChatHelper.h"
#include "Playerbots.h"
#include "Trigger.h"

bool ExternalEventHelper::ParseChatCommand(std::string const command, Player* owner)
{
    // "table" shortcut: rewrite to "cast Ritual of Refreshment" for mage bots
    if (command == "table")
    {
        auto* value = aiObjectContext->GetValue<Unit*>("self target");
        Player* bot = value ? dynamic_cast<Player*>(value->Get()) : nullptr;
        if (bot && bot->getClass() == CLASS_MAGE && bot->GetLevel() >= 70)
            return HandleCommand("cast", "Ritual of Refreshment", owner);

        return true; // silently ignore for non-mages
    }

    if (HandleCommand(command, "", owner))
        return true;

    size_t i = std::string::npos;
    while (true)
    {
        size_t found = command.rfind(" ", i);
        if (found == std::string::npos || !found)
            break;

        std::string const name = command.substr(0, found);
        std::string const param = command.substr(found + 1);

        i = found - 1;

        if (HandleCommand(name, param, owner))
            return true;
    }

    if (!ChatHelper::parseable(command))
        return false;

#if 0 // disable auto inventory/trade response to item links in chat
    HandleCommand("c", command, owner);
    HandleCommand("t", command, owner);
#endif

    return true;
}

void ExternalEventHelper::HandlePacket(std::map<uint16, std::string>& handlers, WorldPacket const& packet,
                                       Player* owner)
{
    uint16 opcode = packet.GetOpcode();
    std::string const name = handlers[opcode];
    if (name.empty())
        return;

    Trigger* trigger = aiObjectContext->GetTrigger(name);
    if (!trigger)
        return;

    WorldPacket p(packet);
    trigger->ExternalEvent(p, owner);
}

bool ExternalEventHelper::HandleCommand(std::string const name, std::string const param, Player* owner)
{
    Trigger* trigger = aiObjectContext->GetTrigger(name);
    if (!trigger)
        return false;

    trigger->ExternalEvent(param, owner);

    return true;
}
