-- phpMyAdmin SQL Dump
-- version 4.0.10deb1
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Jan 28, 2017 at 02:44 PM
-- Server version: 5.5.52-0ubuntu0.14.04.1
-- PHP Version: 5.5.9-1ubuntu4.20

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `monitor`
--

-- --------------------------------------------------------

--
-- Table structure for table `hosts_analysis`
--

CREATE TABLE IF NOT EXISTS `hosts_analysis` (
  `host_name` varchar(40) NOT NULL,
  `cpu_max` int(11) NOT NULL,
  `cpu_max_timestamp` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `cpu_min` int(11) NOT NULL,
  `cpu_min_timestamp` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `memory_max` int(11) NOT NULL,
  `memory_max_timestamp` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `memory_min` int(11) NOT NULL,
  `memory_min_timestamp` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `disk_max` int(11) NOT NULL,
  `disk_max_timestamp` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `disk_min` int(11) NOT NULL,
  `disk_min_timestamp` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  UNIQUE KEY `host_name_2` (`host_name`),
  KEY `host_name` (`host_name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `hosts_details`
--

CREATE TABLE IF NOT EXISTS `hosts_details` (
  `id` int(11) NOT NULL,
  `canonical_name` varchar(40) NOT NULL,
  `ip` varchar(16) NOT NULL,
  `display_name` varchar(4) NOT NULL,
  `grid_y` int(11) NOT NULL,
  `grid_x` int(11) NOT NULL,
  `site` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `hosts_status`
--

CREATE TABLE IF NOT EXISTS `hosts_status` (
  `id` int(11) NOT NULL,
  `status` int(11) NOT NULL,
  `status_timestamp` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `memory_usage` int(11) NOT NULL,
  `memory_timestamp` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `cpu_usage` int(11) NOT NULL,
  `cpu_timestamp` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `disk_usage` int(11) NOT NULL,
  `disk_timestamp` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Triggers `hosts_status`
--
DROP TRIGGER IF EXISTS `update_analysis`;
DELIMITER //
CREATE TRIGGER `update_analysis` AFTER UPDATE ON `hosts_status`
 FOR EACH ROW BEGIN
IF NEW.cpu_usage > hosts_analysis.cpu_max THEN
UPDATE hosts_analysis SET cpu_max = NEW.cpu_usage;
END IF;
END
//
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `sites`
--

CREATE TABLE IF NOT EXISTS `sites` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `code` varchar(6) NOT NULL,
  `name` varchar(30) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=6 ;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
