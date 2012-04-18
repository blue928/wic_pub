<?php
// $Id$

/**
 * @file
 * Admin page callback for the wic_pub module.
 */

/**
 * Menu callback: order form settings form.
 */
function wic_pub_admin_settings() {
  $form['wic_pub_notice_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Order notification recipient email address'),
    '#description' => t('Notifications about submitted orders will be sent to this email address.'),
    '#default_value' => variable_get('wic_pub_notice_email', 'wichq-sfpd@fns.usda.gov'),
    '#required' => FALSE,
  );
	return system_settings_form($form);
}