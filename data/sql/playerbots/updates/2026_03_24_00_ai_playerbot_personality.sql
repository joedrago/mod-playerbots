ALTER TABLE `ai_playerbot_texts` ADD COLUMN `personality` VARCHAR(64) NOT NULL DEFAULT 'default' AFTER `reply_type`;
