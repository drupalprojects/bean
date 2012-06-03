<?php

/**
 * @file
 * Contains callbacks for service resource manipulation.
 */

/**
 * Adds a new bean to a node and returns the bid.
 *
 * @param $bean Bean
 *   An object as would be returned from bean_load().
 * @return
 *   Unique identifier for the bean (bid) or errors if there was a problem.
 */
function bean_services_create($bean) {
  if (empty($bean['type'])) {
    return services_error(t('A type must be provided.'));
  }

  // Include the bean_form.
  module_load_include('inc', 'bean', 'includes/bean.pages');

  $bean_new = bean_create($bean);

  // Setup form_state.
  $form_state = array();
  $form_state['values'] = $bean;
  $form_state['values']['op'] = t('Save');
  $form_state['build_info']['args'] = array(&$bean_new);

  $ret = drupal_form_submit('bean_form', $form_state);

  // Error if needed.
  if ($errors = form_get_errors()) {
    return services_error(implode(" ", $errors), 406, array('form_errors' => $errors));
  }

  $bean = $form_state['bean'];

  return array(
    'bid' => $bean->bid,
    'uri' => services_resource_uri(array('bean', $bean->bid)),
  );
}

/**
 * Updates a bean and returns the bid.
 *
 * @param $bid
 *   Unique identifier for this bean.
 * @param $bean Bean
 *   An object as would be returned from bean_load().
 * @return
 *   Unique identifier for the bean (bid) or FALSE if there was a problem.
 */
function bean_services_update($bid, $bean) {
  // Include the bean_form.
  module_load_include('inc', 'bean', 'includes/bean.pages');

  $bean['bid'] = $bid;

  $old_bean = bean_load($bid);
  if (empty($old_bean)) {
    return services_error(t('Bean @bid not found.', array('@bid' => $bid)), 404);
  }

  // Setup form_state.
  $form_state = array();
  $form_state['values'] = $bean;
  $form_state['values']['op'] = t('Save');
  $form_state['build_info']['args'] = array(&$old_bean);

  drupal_form_submit('bean_form', $form_state);

  if ($errors = form_get_errors()) {
    return services_error(implode(" ", $errors), 406, array('form_errors' => $errors));
  }

  return $bid;
}

/**
 * Returns a specified bean.
 *
 * @param $bid
 *   Unique identifier for the specified bean.
 * @return
 *   The bean object.
 */
function bean_services_retrieve($bid) {
  return bean_load($bid);
}

/**
 * Delete a bean.
 *
 * @param $bid
 *   Unique identifier of the bean to delete.
 * @return
 *   True.
 */
function bean_services_delete($bid) {
  $bean = bean_load($bid);
  if (empty($bean)) {
    return services_error(t('There is no bean found with id @bid.', array('@bid' => $bid)), 404);
  }

  // Delete bean.
  bean_delete($bean);

  // Clear the bean cache so an anonymous user sees that the bean was deleted.
  bean_reset();
  return TRUE;
}

/**
 * Return an array of optionally paged bids based on a set of criteria.
 *
 * An example request might look like:
 *
 * http://domain/endpoint/bean?fields=bid,label&parameters[type]=mytype
 *
 * This would return an array of objects with only bid and label defined, where
 * type = 'mytype'.
 *
 * @param $page
 *   Page number of results to return (in pages of 20).
 * @param $fields
 *   The fields you want returned.
 * @param $parameters
 *   An array containing fields and values used to build a sql WHERE clause
 *   indicating items to retrieve.
 * @param $page_size
 *   Integer number of items to be returned.
 * @return
 *   An array of bean objects.
 **/
function bean_services_index($page, $fields, $parameters, $page_size) {
  $bean_select = db_select('bean', 't')
    ->orderBy('delta', 'ASC');

  services_resource_build_index_query($bean_select, $page, $fields, $parameters, $page_size, 'bean');

  $results = services_resource_execute_index_query($bean_select);

  return services_resource_build_index_list($results, 'bean', 'bid');
}

/**
 * Access callback for the bean resource.
 *
 * @param $op
 *  The operation that's going to be performed.
 * @param $args
 *  The arguments that will be passed to the callback.
 * @return
 *  Whether access is given or not.
 */
function bean_services_access($op, $args) {
  $bean = NULL;
  if (!empty($args[0])) {
    if (is_numeric($args[0])) {
      $bean = bean_load($args[0]);
    }
    else if (is_array($args[0])) {
      $bean = (object) $args[0];
    }
  }

  if (!empty($args[0]) && !is_object($bean)) {
    return FALSE;
  }

  return bean_access($op, $bean);
}
