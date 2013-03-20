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
function wic_pub_order_form($step = NULL) {
	// Check to make sure we can find the publications vocabulary ID.
	if (!($vid = variable_get('wic_pub_of_vid', NULL))) {
		drupal_set_message(t('The WIC publications order form cannot be created.  Please contact a system administrator.'), 'error');
		return NULL;
	}

  // Include required ctools files.
  ctools_include('wizard');
  ctools_include('object-cache');

	// Define the steps of the form from the taxonomy tree.
  $terms = taxonomy_get_tree($vid, 0, 1);
  $order = array();
  $forms = array();
  foreach ($terms as $term) {
    $order[$term->tid] = t('@name', array('@name' => $term->name));
    $forms[$term->tid] = array(
      'form id' => 'wic_pub_order_form_step',
    );
  }
	$order['clinic-info'] = t('WIC clinic information');
	$forms['clinic-info'] = array(
    'form id' => 'wic_pub_order_form_clinic_info'
  );

  // Set the tid for the first step.
  if (empty($step)) {
    reset($order);
    $step = key($order);

    // Clear the Ctool cache object.
    wic_pub_order_form_clear_page_cache('order');
  }

  $form_info = array(
    // Specify unique form id for this form.
    'id' => 'wic_pub_order_form',
    // Specify the path for this form.
    'path' => 'publication-order-form/%step',
    // Show the Back button.
    'show back' => TRUE,
    // Show the Cancel button.
    'show cancel' => TRUE,
    // Callback to use when the 'next' button is clicked.
    'next callback' => 'wic_pub_order_form_next',
    // Callback to use when entire form is completed.
    'finish callback' => 'wic_pub_order_form_finish',
    // Callback to use when user cancels wizard.
    'cancel callback' => 'wic_pub_order_form_cancel',
    // Specify the order of the child forms and their page titles.
    'order' => $order,
    // Define the child forms.
    'forms' => $forms,
  );

  // Make cached data available within each step's $form_state array.
  $form_state['order_object'] = wic_pub_order_form_get_page_cache('order');

  // Pass the vid and tid of the current step to the form.
  $form_state['vid'] = $vid;
  $form_state['tid'] = $step;

  // Return the form as a Ctools multi-step form.
  $output = ctools_wizard_multistep_form($form_info, $step, $form_state);

  return $output;
}

/**
 * Form builder: Publication order form item selection pages.
 */
function wic_pub_order_form_step($form, &$form_state) {
  $vid = $form_state['vid'];
  $tid = $form_state['tid'];

  // Get the second-level terms under the page.  These are sub-headings.
  $headings = taxonomy_get_tree($vid, $tid, 1);

  // Get nodes tagged with the page term.
  $nids = taxonomy_select_nodes($tid, FALSE, FALSE, array('t.created' => 'ASC'));
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

  // Add the validation function.
  $form['#validate'] = array('wic_pub_order_form_step_validate');

  return $form;
}

/**
 * Form builder: Creates order form elements for publication nodes.
 */
function wic_pub_order_form_set(&$form, &$form_state, $group, $nids) {
	foreach ($nids as $nid) {
		$node = node_load($nid);
    // Get the field values.
    $order_numbers = field_get_items('node', $node, 'wic_pub_order_number');
    $order_number = field_view_value('node', $node, 'wic_pub_order_number', $order_numbers[0]);
    $descriptions = field_get_items('node', $node, 'wic_pub_description');
    $description = field_view_value('node', $node, 'wic_pub_description', $descriptions[0]);
    $in_stocks = field_get_items('node', $node, 'wic_pub_in_stock');
    $in_stock = (bool) $in_stocks[0]['value'];
    $max_quantities = field_get_items('node', $node, 'wic_pub_max_quantity');
    $max_quantity = field_view_value('node', $node, 'wic_pub_max_quantity', $max_quantities[0]);
    $max_quantity = (int) $max_quantity['#markup'];

	  $form['items'][$group][$node->nid]['#tree'] = TRUE;
		$form['items'][$group][$node->nid]['quantity'] = array(
			'#type' => 'textfield',
			'#title' => t('Quantity'),
			'#title_display' => 'invisible',
			'#size' => 3,
			'#maxlength' => 3,
			// Disable the quantity field if the item is not in stock.
			'#disabled' => !$in_stock,
			'#default_value' => empty($form_state['order_object']->items[$node->nid]['quantity']) ? '' : $form_state['order_object']->items[$node->nid]['quantity'],
		);
		$form['items'][$group][$node->nid]['printed_order_number'] = array(
		  '#markup' => $order_number['#markup'],
		);
		$form['items'][$group][$node->nid]['printed_description'] = array(
		  '#markup' => theme('wic_pub_order_form_description', array(
			  'description' => $description['#markup'],
				'in_stock' => $in_stock,
				'max_quantity' => $max_quantity,
			)),
		);
		$form['items'][$group][$node->nid]['order_number'] = array(
			'#type' => 'value',
			'#value' => $order_number['#markup'],
		);
		$form['items'][$group][$node->nid]['description'] = array(
			'#type' => 'value',
			'#value' => $description['#markup'],
		);
		$form['items'][$group][$node->nid]['in_stock'] = array(
			'#type' => 'value',
			'#value' => $in_stock,
		);
		$form['items'][$group][$node->nid]['max_quantity'] = array(
			'#type' => 'value',
			'#value' => $max_quantity,
		);
	}
}

/**
 * Returns HTML for publication node descriptions shown on the order form.
 */
function theme_wic_pub_order_form_description($variables) {
	$description = $variables['description'];
	$in_stock = $variables['in_stock'];
	$max_quantity = $variables['max_quantity'];
	if (!$in_stock) {
	  $output = "<span class='line-through'>$description</span> <em>currently out of stock</em>";
	}
	elseif ($max_quantity) {
		$output = "$description <strong>(Limit of $max_quantity)</strong>";
	}
	else {
		$output = $description;
	}
	return $output;
}

/**
 * Returns HTML for publication nodes in the order form formatted as a table.
 */
function theme_wic_pub_order_form_step($variables) {
	$form = $variables['form'];
	// Return if we're on the last page.  The clinic info does not get themed.
	if (isset($form['name'])) {
		return;
	}

  $header = array(t('Quantity'), t('Order number'), t('Description'));
	$output = '';
  foreach (element_children($form['items']) as $group) {
		if ($group != 'no_heading') {
			$output .= '<h3>' . t($group) . "</h3>\n";
		}
		$rows = array();
		foreach (element_children($form['items'][$group]) as $nid) {
			$row = array();
			$row['data'][] = drupal_render($form['items'][$group][$nid]['quantity']);
			$row['data'][] = drupal_render($form['items'][$group][$nid]['printed_order_number']);
			$row['data'][] = drupal_render($form['items'][$group][$nid]['printed_description']);
			$rows[] = $row;
		}

    $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'wic-pub-order-form-table')));
	}
	$output .= drupal_render_children($form);
	return $output;
}

/**
 * Form validation handler: Publication order form item selection pages.
 */
function wic_pub_order_form_step_validate($form, &$form_state) {
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
		elseif ($item['max_quantity'] > 0 && $item['quantity'] > $item['max_quantity']) {
			form_set_error("$nid][quantity", t('The quantity you entered is higher than the maximum.  Refer to the item description for the maximum quantity.'));
			continue;
		}
	}
}

/**
 * Form builder: Publication order form clinic contact info page.
 */
function wic_pub_order_form_clinic_info($form, &$form_state) {
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
  return $form;
}

/**
 * Callback executed when the 'next' button is clicked.
 */
function wic_pub_order_form_next(&$form_state) {
  // Store submitted data in a ctools cache object.
  foreach ($form_state['values'] as $nid => $item) {
    if (is_array($item) && $item['in_stock'] && !empty($item['quantity'])) {
      $form_state['order_object']->items[$nid] = $item;
    }
    elseif (isset($form_state['order_object']->items[$nid])) {
      // If the quantity was deleted, unset the item value in the order.
      unset($form_state['order_object']->items[$nid]);
    }
  }
  wic_pub_order_form_set_page_cache('order', $form_state['order_object']);
}

/**
 * Callback executed when the 'cancel' button is clicked.
 */
function wic_pub_order_form_cancel(&$form_state) {
  // Clear the ctools cache object.
  wic_pub_order_form_clear_page_cache('order');
}

/**
 * Callback executed when the entire form submission is finished.
 */
function wic_pub_order_form_finish(&$form_state) {
	$items = isset($form_state['order_object']->items) ? $form_state['order_object']->items : array();
	// If no items were selected, redirect the user back to the start.
	if (count($items) == 0) {
		drupal_set_message(t('You must specify a quantity for at least one publication to submit this form.'));
		$form_state['redirect'] = 'publication-order-form';
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

  // Clear the Ctool cache object.
  wic_pub_order_form_clear_page_cache('order');

  // Redirect the user to the front page.
  drupal_goto('<front>');
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

/**
 * Retreives an object from the cache.
 *
 * @param string $name
 *  The name of the cached object to retreive.
 */
function wic_pub_order_form_get_page_cache($name) {
  ctools_include('object-cache');
  $cache = ctools_object_cache_get('wic_pub_order_form', $name);

  // If the cached object doesn't exist yet, create an empty object.
  if (!$cache) {
    $cache = new stdClass();
    $cache->locked = ctools_object_cache_test('wic_pub_order_form', $name);
  }

  return $cache;
}

/**
 * Creates or updates an object in the cache.
 *
 * @param string $name
 *  The name of the object to cache.
 *
 * @param object $data
 *  The object to be cached.
 */
function wic_pub_order_form_set_page_cache($name, $data) {
  ctools_include('object-cache');
  $cache = ctools_object_cache_set('wic_pub_order_form', $name, $data);
}

/**
 * Removes an item from the object cache.
 *
 * @param string $name
 *  The name of the object to destroy.
 */
function wic_pub_order_form_clear_page_cache($name) {
  ctools_include('object-cache');
  ctools_object_cache_clear('wic_pub_order_form', $name);
}
