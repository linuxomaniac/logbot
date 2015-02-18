CREATE TABLE IF NOT EXISTS `logs` (
  `user` varchar(30) NOT NULL,
  `length` int(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

ALTER TABLE `logs`
 ADD UNIQUE KEY `user` (`user`);
