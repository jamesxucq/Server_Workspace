/*
Navicat MySQL Data Transfer

Source Server         : 10.0.0.103
Source Server Version : 50623
Source Host           : 10.0.0.103:3306
Source Database       : AuthDB

Target Server Type    : MYSQL
Target Server Version : 50623
File Encoding         : 65001

Date: 2016-01-15 17:12:32
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `ssev`
-- ----------------------------
DROP TABLE IF EXISTS `ssev`;
CREATE TABLE `ssev` (
  `Cache_id` char(32) NOT NULL,
  `Serv_addr` char(32) NOT NULL,
  `Bind_port` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of ssev
-- ----------------------------
INSERT INTO `ssev` VALUES ('10.0.0.103:8089', '10.0.0.103', '8090');

-- ----------------------------
-- Table structure for `user`
-- ----------------------------
DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
  `User_id` int(20) unsigned NOT NULL AUTO_INCREMENT,
  `User_name` char(32) NOT NULL,
  `Password` char(96) NOT NULL,
  `Salt` char(32) NOT NULL,
  `Pool_size` int(11) unsigned NOT NULL,
  `Location` text NOT NULL,
  `Sync_locked` int(11) unsigned NOT NULL DEFAULT '0',
  `Auth_host` char(32) NOT NULL,
  `Access_key` char(64) NOT NULL,
  `Lease_time` bigint(20) NOT NULL,
  `Version` char(32) NOT NULL,
  `Linked_status` text,
  PRIMARY KEY (`User_id`,`User_name`,`Password`)
) ENGINE=MyISAM AUTO_INCREMENT=22 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of user
-- ----------------------------
INSERT INTO `user` VALUES ('8', 'ping_auth', 'Z2Q1MEQ4OXFzUXNKYnlJSTEyMzQ1NmdkNTBEODlxc1FzSmJ5SUk=', 'gd50D89qsQsJbyII', '0', '', '0', '', '', '0', '', '<client name,link status,action time,data bomb>');
INSERT INTO `user` VALUES ('1', 'xujiecq@163.com', 'c2RoRlRKVTJWVFU4UWU4WTEyMzQ1NnNkaEZUSlUyVlRVOFFlOFk=', 'sdhFTJU2VTU8Qe8Y', '6', '/home/james/workspace/sd_users/xujiecq@163.com', '0', '10.0.0.103:8089', 'FlQEqgi7TY20MTtXN52U', '1446908608', '1.21.1', '<\"PC2012061115TZZ\",\"linked\",1349162185,\"false\"><\"YOUR-42B0C06E12\",\"initial\",1446911037,\"false\">');
INSERT INTO `user` VALUES ('2', 'xujie@163.com', 'c2RoRlRKVTJWVFU4UWU4WTEyMzQ1NnNkaEZUSlUyVlRVOFFlOFk=', 'sdhFTJU2VTU8Qe8Y', '6', '/home/james/workspace/sd_users/xujie@163.com', '0', '10.0.0.103:8089', 'hneMrUofnmjJoIPmas9U', '1452159592', '1.21.1', '<\"PC-201410261749\",\"linked\",1439787452,\"false\"><\"PC-201306120827\",\"initial\",1446958577,\"false\"><\"YOUR-42B0C06E12\",\"linked\",1446958615,\"false\">');
INSERT INTO `user` VALUES ('21', 'david', 'VnZsUnRLT0p3aTdwckJ4bTEyMzQ1NlZ2bFJ0S09Kd2k3cHJCeG0=', 'VvlRtKOJwi7prBxm', '6', '/home/james/workspace/sd_users/david', '0', '10.0.0.103:8089', 'MAXMAXMAX', '1446908608', '1.21.1', null);
