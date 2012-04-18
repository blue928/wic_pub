<?php
// $Id$

/**
 * @file
 * Contains functions to create the order form for the wic_pub module.
 */

/**
 * Menu callback: publication order form
 *
 * Creates a multi-step form for ordering WIC Publications.
 *
 * The layout of the form is determined from the Publications order form
 * terms taxonomy. Top-level terms are rendered as pages. Second-level
 * terms are rendered as sub-headings on the page.
 */
function wic_pub_order_form($form, &$form_state) {
	// Check to make sure we can find the publications vocabulary ID.
	if (!($vid = variable_get('wic_pub_of_vid', NULL))) {
		drupal_set_message(t('The WIC publications order form cannot be created.  Please contact a system administrator.'), 'error');
		return NULL;
	}

	// Set the current page number.  The submit functions will increment/decrement the value.
	$form_state['page_num'] = empty($form_state['page_num']) ? 1 : $form_state['page_num'];
	// Get the top-level taxonomy terms.  These are our form pages.
	$pages = taxonomy_get_tree($vid, 0, 1);
	if ($form_state['page_num'] <= count($pages)) {
		// Default to the first page.
		$page = reset($pages);
		// Advance the page to the current page.
		for ($i = 1; $i < $form_state['page_num']; $i++) {
			$page = next($pages);
		}
		// Set the page title to the name of the top-level taxonomy term.
		drupal_set_title(t($page->name));
		// Get the second-level terms under the page.  These are sub-headings.
		$headings = taxonomy_get_tree($vid, $page->tid, 1);
		
		// Get nodes tagged with the page term.
		$nids = taxonomy_select_nodes($page->tid, FALSE, FALSE, array('t.created' => 'ASC'));
		if (!empty($nids)) {
			// Construct form elements from the nodes. Add the 'items' array so that
			// element_children() in the theme function doesn't return form metadata
			// elements as headings.
			wic_pub_order_form_set($form, $form_state, 'no_heading', $nids);
		}
		foreach ($headings as $heading) {
			$nids = taxonomy_select_nodes($heading->tid, FALSE, FALSE, array('t.created' => 'ASC'));
			if (!empty($nids)) {
				wic_pub_order_form_set($form, $form_state, $heading->name, $nids);
			}
		}
	}
	// The page number is greater than the number of array items, display the
	// WIC clinic info form.
	else {
		drupal_set_title(t('WIC clinic information'));
		wic_pub_order_form_last($form, $form_state);
	}
	// Show the back button if we're not on the first page of the form.
	if ($form_state['page_num'] > 1) {
		$form['back'] = array(
			'#type' => 'submit',
			'#value' => t('Back'),
			// Only validate on "Next", but don't limit validation errors on "Back"
			// or the form values won't be saved.
			'#submit' => array('wic_pub_back_submit'),
		);
	}
	// Show the next button if we're not on the last page of the form.
	if ($form_state['page_num'] <= count($pages)) {
		$form['next'] = array(
			'#type' => 'submit',
			'#value' => t('Next'),
			'#validate' => array('wic_pub_next_validate'),
			'#submit' => array('wic_pub_next_submit'),
		);
	}
	else {
		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Submit'),
			'#submit' => array('wic_pub_order_form_submit'),
		);
	}
	return $form;
}

/**
 * Form builder: publication order form item selection pages
 */
function wic_pub_order_form_set(&$form, &$form_state, $group, $nids) {
	foreach ($nids as $nid) {
		$node = node_load($nid);
	  $form['items'][$group][$node->nid]['#tree'] = TRUE;
		$form['items'][$group][$node->nid]['quantity'] = array(
			'#type' => 'textfield',
			'#title' => t('Quantity'),
			'#title_display' => 'invisible',
			'#size' => 3,
			'#maxlength' => 3,
			// Disable the quantity field if the item is not in stock.
			'#disabled' => !$node->wic_pub_in_stock,
			'#default_value' => empty($form_state['values'][$node->nid]['quantity']) ? '' : $form_state['values'][$node->nid]['quantity'],
		);
		$form['items'][$group][$node->nid]['printed_order_number'] = array(
		  '#markup' => $node->wic_pub_order_number,
		);
		$form['items'][$group][$node->nid]['printed_description'] = array(
		  '#markup' => theme('wic_pub_order_form_description', array(
			  'description' => $node->wic_pub_description,
				'in_stock' => $node->wic_pub_in_stock,
				'quantity_max' => $node->wic_pub_quantity_max,
			)),
		);
		$form['items'][$group][$node->nid]['order_number'] = array(
			'#type' => 'value',
			'#value' => $node->wic_pub_order_number,
		);
		$form['items'][$group][$node->nid]['description'] = array(
			'#type' => 'value',
			'#value' => $node->wic_pub_description,
		);
		$form['items'][$group][$node->nid]['in_stock'] = array(
			'#type' => 'value',
			'#value' => $node->wic_pub_in_stock,
		);
		$form['items'][$group][$node->nid]['quantity_max'] = array(
			'#type' => 'value',
			'#value' => $node->wic_pub_quantity_max,
		);
	}
}

/**
 * Form builder: publication order form clinic contact info page
 */
function wic_pub_order_form_last(&$form, &$form_state) {
	$form['name'] = array(
		'#type' => 'textfield',
		'#title' => t('Name'),
		'#maxlength' => 60,
		'#required' => TRUE,
		'#default_value' => empty($form_state['values']['name']) ? '' : $form_state['values']['name'],
	);
	$form['clinic_name'] = array(
		'#type' => 'textfield',
		'#title' => t('WIC clinic name'),
		'#required' => TRUE,
		'#default_value' => empty($form_state['values']['clinic_name']) ? '' : $form_state['values']['clinic_name'],
	);
	$form['address'] = array(
		'#type' => 'textfield',
		'#title' => t('Street address'),
		'#maxlength' => 60,
		'#required' => TRUE,
		'#default_value' => empty($form_state['values']['address']) ? '' : $form_state['values']['address'],
	);
	$form['city'] = array(
		'#type' => 'textfield',
		'#title' => t('City, state, zip'),
		'#maxlength' => 60,
		'#required' => TRUE,
		'#default_value' => empty($form_state['values']['city']) ? '' : $form_state['values']['city'],
	);
	$form['phone'] = array(
		'#type' => 'textfield',
		'#title' => t('Phone'),
		'#size' => 14,
		'#maxlength' => 12,
		'#required' => TRUE,
		'#default_value' => empty($form_state['values']['phone']) ? '' : $form_state['values']['phone'],
	);
	$form['email'] = array(
		'#type' => 'textfield',
		'#title' => t('Email'),
		'#maxlength' => 60,
		'#required' => TRUE,
		'#default_value' => empty($form_state['values']['email']) ? '' : $form_state['values']['email'],
	);
}

/**
 * Form validation handler: publication order form item selection pages
 */
function wic_pub_next_validate($form, &$form_state) {
	foreach ($form_state['values'] as $nid => $item) {
		if (!is_array($item) || empty($item['quantity'])) {
			continue;
		}
		// Just clear any quantities for items that are not in stock.
		if (!$item['in_stock']) {
			$item['quantity'] = '';
			continue;
		}
		// Validate the quantity.
		if (!is_numeric($item['quantity']) || !is_int((int) $item['quantity'])) {
			form_set_error("$nid][quantity", t("The quantity you enter must be an integer."));
			continue;
		}
		elseif ($item['quantity_max'] > 0 && $item['quantity'] > $item['quantity_max']) {
			form_set_error("$nid][quantity", t('The quantity you entered is higher than the maximum.  Refer to the item description for the maximum quantity.'));
			continue;
		}
	}
}

/**
 * Form submit handler: publication order form back button
 */
function wic_pub_back_submit($form, &$form_state) {
	$page_num = &$form_state['page_num'];
	// Store the values from this page.
	$form_state['pages'][$page_num] = $form_state['values'];

  if ($page_num > 1) {
		// Set the page number.
		$page_num--;
		// Restore the values for the previous page.
		$form_state['values'] = $form_state['pages'][$page_num];
	}
  $form_state['rebuild'] = TRUE;
}

/**
 * Form submit handler: publication order form next button
 */
function wic_pub_next_submit($form, &$form_state) {
	$page_num = &$form_state['page_num'];
	// Store the values from this page.
	$form_state['pages'][$page_num] = $form_state['values'];

	// Set the page number.
	$page_num++;
	// Restore the values for the next page, if they exist.
	if (!empty($form_state['pages'][$page_num])) {
		$form_state['values'] = $form_state['pages'][$page_num];
	}
	else {
		$form_state['values'] = array();
	}
  $form_state['rebuild'] = TRUE;
}

/**
 * Form submit handler: publication order form
 */
function wic_pub_order_form_submit($form, &$form_state) {
	$items = array();
	foreach ($form_state['pages'] as $page) {
		foreach ($page as $nid => $item) {
			// Load each item with a set quantity into $items.
			if (is_array($item) && $item['in_stock'] && !empty($item['quantity'])) {
				$items[$nid] = $item;
			}
		}
	}
	// If no items were selected, redirect the user back to the start.
	if (count($items) == 0) {
		drupal_set_message(t('You must specify a quantity for at least one publication to submit this form.'));
		$form_state['redirect'] = 'publication_order_form';
		return;
	}

	// Prepare to send emails to the user and to WIC.
	wic_pub_order_sanitize($form_state['values']);
	$params = $form_state['values'];
	$params['items'] = $items;
	// Send the order confirmation email to the user.
	drupal_mail('wic_pub', 'order_confirm', $form_state['values']['email'], 'en', $params);
	// Send the order notification email to WIC.
	drupal_mail('wic_pub', 'order_notice', variable_get('wic_pub_notice_email', 'wichq-sfpd@fns.usda.gov'), 'en', $params);
	drupal_set_message(t('Thank you for your order! You should receive a confirmation email soon.'));
	$form_state['redirect'] = '<front>';
}

/**
 * Sanitize order fields to prevent cross-site scripting.
 *
 * We're only sanitizing contact info fields here, not the $item arrays
 * in the form submit function. Items should be clean from validation.
 */
function wic_pub_order_sanitize(&$contact_info) {
	foreach ($contact_info as $key => $value) {
    $contact_info[$key] = filter_xss($value, array());
	}
}